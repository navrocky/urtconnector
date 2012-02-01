
#include <boost/bind.hpp>

#include <QTreeWidget>
#include <QHeaderView>
#include <QAction>
#include <QMessageBox>

#include <common/qt_syslog.h>
#include <common/server_bookmark.h>
#include <common/item_view_dblclick_action_link.h>
#include <common/qaccumulatingconnection.h>
#include <common/tools.h>
#include <jobs/job_queue.h>
#include "../server_options_dialog.h"
#include "../app_options.h"
#include "../job_update_selected.h"
#include "status_item_delegate.h"
#include "common_item_tags.h"
#include "visible_updater.h"

#include "bookmark_tab.h"
#include "main_window.h"
#include "tools.h"

#include <remote/remote.h>
#include <remote/manager.h>

#include "common/qt_syslog.h"

SYSLOG_MODULE(bookmark_tab)

using namespace remote;

#define SYNC_DEBUG LOG_DEBUG << " >> " << __FUNCTION__ << ": "

remote::group get_group(const tab_context& ctx){
    
    SYNC_DEBUG;
    
    remote::group obj("bookmarks");

    BOOST_FOREACH( const server_bookmark& bm, ctx.bookmarks()->list() ) {
        obj << bm;
        SYNC_DEBUG << bm.sync_stamp().toString();
    }  
    return obj;
}

void set_group(const tab_context& ctx, const remote::group& remote){
    remote::group obj("bookmarks");

    SYNC_DEBUG;
    
    BOOST_FOREACH( const server_bookmark& bm, ctx.bookmarks()->list() ) {
        obj << bm;
        SYNC_DEBUG << bm.sync_stamp().toString();
    }  

    remote::group::Entries merged = remote::merge( obj.entries(), remote.entries() );
    
    BOOST_FOREACH( const remote::intermediate& imd, merged ) {
        server_bookmark bm;
        bm.load( imd.save() );
        SYNC_DEBUG << "adding bookmark:"<<bm.id().address();
        ctx.bookmarks()->add(bm);
    }
}

////////////////////////////////////////////////////////////////////////////////
// bookmark_tab

bookmark_tab::bookmark_tab(const QString& object_name,
                           const tab_context& ctx,
                           QWidget* parent)
: server_list_common_tab(object_name, tr("Favorites"), ctx, false, parent)
, updater_(new visible_updater(this, SLOT(update_contents()), this))
{
    setWindowIcon(QIcon("icons:bookmarks.png"));

    add_action_ = new QAction(QIcon("icons:add.png"), tr("Add"), this);
    add_action_->setToolTip("Add new favorite");
    connect(add_action_, SIGNAL(triggered()), SLOT(add()));

    edit_action_ = new QAction(QIcon("icons:edit.png"), tr("Edit"), this);
    connect(edit_action_, SIGNAL(triggered()), SLOT(edit_selected()));

    remove_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove"), this);
    connect(remove_action_, SIGNAL(triggered()), SLOT(remove_selected()));

    refresh_selected_ = new QAction(QIcon("icons:view-refresh.png"), tr("Refresh selected"), this);
    connect(refresh_selected_, SIGNAL(triggered()), SLOT(refresh_selected()));

    refresh_all_ = new QAction(QIcon("icons:download.png"), tr("Refresh all"), this);
    connect(refresh_all_, SIGNAL(triggered()), SLOT(refresh_all()));

    QList<QAction*> acts;
    
    acts << add_action_
        << edit_action_
        << remove_action_
        << add_separator_action(this)
        << refresh_selected_
        << refresh_all_;

//     QAction* exp = new QAction(QIcon(), tr("export"), this);
//     connect(exp, SIGNAL(triggered()), SLOT(test_export()));
    
//     QAction* import = new QAction(QIcon(), tr("import"), this);
//     connect(import, SIGNAL(triggered()), SLOT(test_import()));

//     acts << exp << import;
        
    addActions(acts);

    tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree()->addAction(context().connect_action());
    add_separator_action(tree());

    tree()->addActions( actions() );

    new item_view_dblclick_action_link(this, tree(), ctx.connect_action());

    new QAccumulatingConnection(context().bookmarks().get(), SIGNAL(changed( const server_bookmark&, const server_bookmark& )),
                                updater_, SLOT(update_contents()), 100,
                                QAccumulatingConnection::Finally,
                                this);
    new QAccumulatingConnection(context().serv_list().get(), SIGNAL(changed()),
                                updater_, SLOT(update_contents()), 200,
                                QAccumulatingConnection::Periodically,
                                this);

    update_actions();
    
    
    //registering Object in syncro_manager
    syncro_manager::Getter g = boost::bind(get_group, boost::ref(context()));
    syncro_manager::Setter s = boost::bind(set_group, boost::ref(context()), _1);
    context().syncro().attach("bookmarks", g, s, "bookmarks");
}

void bookmark_tab::do_selection_change()
{
    server_list_common_tab::do_selection_change();
    update_actions();
}

void bookmark_tab::update_servers_info()
{
    LOG_DEBUG << "Update servers info";
    for (int i = 0; i < tree()->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tree()->topLevelItem(i);
        update_server_info_item(context(), item);
    }
}

void bookmark_tab::update_contents()
{
    LOG_DEBUG << "Update contents";
    QTreeWidgetItem* cur_item = tree()->currentItem();

    tree()->setUpdatesEnabled(false);
    tree()->setSortingEnabled(false);

    const server_bookmark_list::bookmark_map_t& bms = context().bookmarks()->list();

    // who changed, appeared?
    foreach (const server_bookmark& bm, bms)
    {
        const server_id& id = bm.id();
        server_items::iterator it2 = items_.find(id);
        QTreeWidgetItem* item;

        if (it2 == items_.end())
        {
            item = new QTreeWidgetItem( tree() );
            item->setData( 0, c_id_role, QVariant::fromValue(id) );
            items_[id] = item;
        }
    }

    // remove old items
    for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
    {
        const server_id& id = it->first;
        if ( bms.find(id) != bms.end() )
            continue;
        QTreeWidgetItem* item = it->second;
        if (item == cur_item)
            cur_item = 0;
        delete item;
        items_.erase(it);
    }

    update_servers_info();

    tree()->setSortingEnabled(true);
    tree()->setUpdatesEnabled(true);

    if (tree()->topLevelItemCount() > 0 && cur_item && app_settings().center_current_row())
        tree()->scrollToItem(cur_item, QAbstractItemView::PositionAtCenter);

    set_total_count(items_.size());
    filter_changed();
    update_actions();
}

void bookmark_tab::add()
{
    server_options_dialog d(this);
    d.set_server_list(context().serv_list());
    d.set_update_params(context().geo(), context().job_que());
    if (d.exec() == QDialog::Rejected)
        return;
    context().bookmarks()->add(d.options());
}

void bookmark_tab::edit_selected()
{
    server_id id = selected_server();
    if (id.is_empty())
        return;
    const server_bookmark& bm = context().bookmarks()->get(id);
    server_options_dialog d(this, bm);
    d.set_server_list(context().serv_list());
    d.set_update_params(context().geo(), context().job_que());
    if (d.exec() == QDialog::Rejected)
        return;
    context().bookmarks()->change(id, d.options());
}

void bookmark_tab::remove_selected()
{
    const server_id_list& sel = selection();
    if (sel.size() == 0)
        return;
    if (QMessageBox::question(this, tr("Delete a favorite"), tr("Delete selected favorite(s)?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;

    foreach (const server_id& id, sel)
    {
        context().bookmarks()->remove(id);
    }
}

void bookmark_tab::refresh_all()
{
    server_id_list l;

    foreach(const server_bookmark& bm, context().bookmarks()->list())
    {
        l.push_back(bm.id());
    }

    context().job_que()->add_job(job_p(new job_update_selected(l,
        context().serv_list(), *context().geo())));
}

void bookmark_tab::update_actions()
{
    const server_id& id = selected_server();
    edit_action_->setEnabled(!id.is_empty());
    remove_action_->setEnabled(!id.is_empty());
    refresh_selected_->setEnabled(!id.is_empty());
    refresh_all_->setEnabled(context().bookmarks()->list().size() > 0);
}


// syncro_manager::Object s_obj;
// 
// void bookmark_tab::test_import(){
//    
// 
//     SYNC_DEBUG << "1";
//     remote::syncro_manager& syncro = context().syncro();
// 
//     SYNC_DEBUG << "2";
//     
//     syncro_manager::Service gdocs_service = *syncro.services().begin();
//     
// 
//     
//     QVariantMap set;
//     
//     set["mail"] = "kinnalru@gmail.com";
//     set["password"] = "malder22";
// 
//         
//     SYNC_DEBUG << "3";
//     if (syncro.storages().empty())
//     {
//         syncro.create(gdocs_service, "sec_plugin", set);
//     }
//     
//     syncro_manager::Storage storage = *syncro.storages().begin();
//     
//     
//     SYNC_DEBUG << "4";
//     syncro_manager::Getter g = boost::bind(get_group, boost::ref(context()));
//     
//     syncro_manager::Setter s = boost::bind(set_group, boost::ref(context()), _1);
//     
//     
//     if (!s_obj)
//     {
//         SYNC_DEBUG << "5";
//         s_obj = syncro.attach("bookmarks", g, s, "bookmarks");
//     }
//     
//     SYNC_DEBUG << "6";
//     
//     syncro.bind(s_obj, storage);
//     
//     SYNC_DEBUG << "7";
//     
//     syncro.sync(s_obj);
//     
//     SYNC_DEBUG << "8";
//     
// }
// 
// 
// 
// void bookmark_tab::test_export(){
// 
//     SYNC_DEBUG << "1";
//     remote::syncro_manager& syncro = context().syncro();
// 
//     SYNC_DEBUG << "2";
//     
//     syncro_manager::Service gdocs_service = *syncro.services().begin();
//     
//     QVariantMap set;
//     
//     set["mail"] = "kinnalru@gmail.com";
//     set["password"] = "malder22";
//     
//     
//     SYNC_DEBUG << "3";
// 
//     if (syncro.storages().empty())
//     {
//         syncro.create(gdocs_service, "sec_plugin", set);
//     }
//     
//     syncro_manager::Storage storage = *syncro.storages().begin();
//     
//     
//     
//     SYNC_DEBUG << "4";
// 
//     
//     
//     if (!s_obj)
//     {
//         SYNC_DEBUG << "5";
//         s_obj = syncro.attach("bookmarks", g, s, "bookmarks");
//     }
//     
//     SYNC_DEBUG << "6";
//     
//     syncro.bind(s_obj, storage);
//     
//     SYNC_DEBUG << "7";
//     
//     syncro.put(s_obj);
//     
//     SYNC_DEBUG << "8";
// }
