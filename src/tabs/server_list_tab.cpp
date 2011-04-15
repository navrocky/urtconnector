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
#include <jobs/job_queue.h>

#include "../app_options.h"
#include "../server_options_dialog.h"
#include "../job_update_from_master.h"
#include "status_item_delegate.h"
#include "common_item_tags.h"
#include "visible_updater.h"
#include "tools.h"

#include "server_list_tab.h"

SYSLOG_MODULE(server_list_tab)

////////////////////////////////////////////////////////////////////////////////
// server_list_tab

server_list_tab::server_list_tab(const QString& object_name,
                                 const tab_context& ctx,
                                 QWidget* parent)
: server_list_common_tab(object_name, tr("All servers"), ctx, parent)
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

    addAction(add_bookmark_action_);
    add_separator_action(this);
    addAction(refresh_selected_action_);
    addAction(refresh_from_master_action_);
    add_separator_action(this);
    addAction(clear_all_action_);
    
    tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree()->addAction(context().connect_action());
    add_separator_action(tree());
    tree()->addAction(add_bookmark_action_);
    add_separator_action(tree());
    tree()->addAction(refresh_selected_action_);
    tree()->addAction(refresh_from_master_action_);
    add_separator_action(tree());
    tree()->addAction(clear_all_action_);

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

    tree()->setUpdatesEnabled(false);
    tree()->setSortingEnabled(false);

    const server_info_list& sil = server_list()->list();

    // who changed, appeared?
    foreach (server_info_list::const_reference r, sil)
    {
        const server_id& id = r.first;
        server_items::iterator it2 = items_.find(id);
        QTreeWidgetItem* item;

        if (it2 == items_.end())
        {
            item = new QTreeWidgetItem( tree() );
            item->setData( 0, c_id_role, QVariant::fromValue(id) );
            items_[id] = item;
        }
        else
            item = it2->second;
        update_server_info_item(context(), item);
    }

    // remove old items
    QList<server_id> to_remove;
    for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
    {
        const server_id& id = it->first;
        if ( sil.find(id) != sil.end() )
            continue;
        QTreeWidgetItem* item = it->second;
        delete item;
        items_.erase(it);
    }

    tree()->setSortingEnabled(true);
    tree()->setUpdatesEnabled(true);

    if (tree()->topLevelItemCount() > 0 && cur_item && app_settings().center_current_row())
        tree()->scrollToItem(cur_item, QAbstractItemView::PositionAtCenter);

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
