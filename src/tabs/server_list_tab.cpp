#include "server_list_tab.h"

#include <boost/bind.hpp>

#include <QTreeWidget>
#include <QHeaderView>
#include <QAction>
#include <QIcon>

#include <common/qt_syslog.h>
#include <common/server_list.h>
#include <common/server_info.h>
#include <common/server_bookmark.h>
#include <common/qaccumulatingconnection.h>
#include <common/tools.h>
#include <common/item_view_dblclick_action_link.h>
#include <common/tree_smart_updater.h>
#include <jobs/job_queue.h>

#include "../app_options.h"
#include "../server_options_dialog.h"
#include "../job_update_from_master.h"
#include "status_item_delegate.h"
#include "common_item_tags.h"
#include "visible_updater.h"
#include "tools.h"

SYSLOG_MODULE(server_list_tab)

////////////////////////////////////////////////////////////////////////////////
// server_list_tab

server_list_tab::server_list_tab(const QString& object_name,
                                 const tab_context& ctx,
                                 QWidget* parent)
: server_list_common_tab(object_name, tr("All servers"), ctx, true, parent)
, updater_(new visible_updater(this, SLOT(update_contents()), this))
{
    setWindowIcon(QIcon("icons:earth.png"));

    add_bookmark_action_ = new QAction(QIcon("icons:bookmarks.png"), tr("Add to favorites"), this);
    add_bookmark_action_->setToolTip(tr("Add selected server to favorites list"));
    connect(add_bookmark_action_, SIGNAL(triggered()), SLOT(add_to_favorites()));

    refresh_selected_action_ = new QAction(QIcon("icons:view-refresh.png"), tr("Refresh selected"), this);
    refresh_selected_action_->setToolTip("Refresh selected server(s)");
    connect(refresh_selected_action_, SIGNAL(triggered()), SLOT(refresh_selected()));

    refresh_from_master_action_ = new QAction(QIcon("icons:download.png"), tr("Update from master server"), this);
    connect(refresh_from_master_action_, SIGNAL(triggered()), SLOT(refresh_all()));

    clear_all_action_ = new QAction(QIcon("icons:edit-clear.png"), tr("Clear all"), this);
    connect(clear_all_action_, SIGNAL(triggered()), SLOT(clear_all()));

    
    QList<QAction*> acts;
    
    acts << add_bookmark_action_
        << add_separator_action(this)
        << refresh_selected_action_
        << refresh_from_master_action_
        << add_separator_action(this)
        << clear_all_action_;

    addActions( acts );
    
    //install actions to tree popup-context
    tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    
    //copy self actions to tree popup-context
    tree()->addActions( actions() );

    new item_view_dblclick_action_link(this, tree(), ctx.connect_action());

    new QAccumulatingConnection(context().serv_list().get(), SIGNAL(changed()),
                                updater_, SLOT(update_contents()), 200,
                                QAccumulatingConnection::Periodically,
                                this);
    update_actions();
}

void server_list_tab::update_contents()
{
    LOG_DEBUG << "Update contents";
    QTreeWidgetItem* cur_item = tree()->currentItem();

    server_id cur_item_id;
    if (cur_item)
        cur_item_id = cur_item->data(0, c_id_role).value<server_id>();

    const server_info_list& sil = server_list()->list();

    // TODO make this code smarter ===
    QVector<server_id> l;
    l.reserve(server_list()->list().size());
    foreach (server_info_list::const_reference r, server_list()->list())
    {
        l.append(r.first);
    }
    // ===

    smart_update_tree_contents(l, c_id_role, tree(), NULL,
        boost::bind(&server_list_tab::update_item, this, _1, _2), items_, false, server_info_item_adapter());

    if (tree()->topLevelItemCount() > 0 && !cur_item_id.is_empty() && app_settings().center_current_row())
    {
        server_items::iterator it = items_.find(cur_item_id);
        if (it != items_.end())
            tree()->scrollToItem(it.value(), QAbstractItemView::PositionAtCenter);
    }

    set_total_count(items_.size());
    filter_changed();
    update_actions();
}

void server_list_tab::add_to_favorites()
{
    const server_id& id = selected_server();
    server_bookmark bm;
    bm.set_id(id);
    server_info_p si = context().serv_list()->get(id);
    if (si)
        bm.set_name(si->name);

    server_options_dialog d(this, bm);
    d.set_server_list(context().serv_list());
    d.set_update_params(context().geo(), context().job_que());
    if (d.exec() == QDialog::Rejected)
        return;
    context().bookmarks()->add(d.options());
}

void server_list_tab::refresh_all()
{
    LOG_DEBUG << "Refresh server list";
    context().job_que()->add_job(job_p(
        new job_update_from_master(context().serv_list(), *context().geo())));
}

void server_list_tab::clear_all()
{
    LOG_DEBUG << "Clear server list";
    context().serv_list()->remove_all();
}

void server_list_tab::do_selection_change()
{
    server_list_common_tab::do_selection_change();
    update_actions();
}

void server_list_tab::update_actions()
{
    const server_id& id = selected_server();
    add_bookmark_action_->setEnabled(!id.is_empty());
    refresh_selected_action_->setEnabled(!id.is_empty());
    clear_all_action_->setEnabled(context().serv_list()->list().size() > 0);
}

void server_list_tab::update_item(QTreeWidgetItem* item, const server_id& id )
{
    update_server_info_item(context(), item);
}
