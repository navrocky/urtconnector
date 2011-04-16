#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QSettings>

#include <common/qt_syslog.h>
#include <common/server_list.h>
#include <jobs/job_queue.h>
#include <geoip/geoip.h>

#include "../job_update_selected.h"

#include "status_item_delegate.h"
#include "common_item_tags.h"

#include "server_list_common_tab.h"

SYSLOG_MODULE(server_list_common_tab)

////////////////////////////////////////////////////////////////////////////////
// server_list_common_tab

server_list_common_tab::server_list_common_tab(const QString& object_name,
                                               const QString& caption,
                                               const tab_context& ctx,
                                               QWidget* parent)
    : filtered_tab(tab_settings_p(new tab_settings(object_name)), ctx, parent)
    , caption_(caption)
    , visible_count_(0)
    , total_count_(0)
    , sls_( settings() )
{
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);

    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->setEditTriggers(QAbstractItemView::EditKeyPressed);
    tree_->setAlternatingRowColors(true);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setRootIsDecorated(false);
    tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(do_selection_change()));

    QTreeWidgetItem *hi = tree_->headerItem();
    hi->setText(7, tr("Address"));
    hi->setText(6, tr("Players"));
    hi->setText(5, tr("Map"));
    hi->setText(4, tr("Game mode"));
    hi->setText(3, tr("Ping"));
    hi->setText(2, tr("Country"));
    hi->setText(1, tr("Name"));
    hi->setText(0, tr("Status"));
    hi->setToolTip(2, tr("Server address (ip:port)"));
    hi->setToolTip(1, tr("Server name"));
    hi->setToolTip(0, tr("Server status"));

    tree_->setItemDelegateForColumn( 0, new status_item_delegate( server_list(), this) );

    QHeaderView* hdr = tree_->header();

//    hdr->moveSection(2, 7);
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(2, 50);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(6, 60);
    hdr->setSortIndicator(3, Qt::AscendingOrder);

    update_caption();
}

void server_list_common_tab::filter_changed()
{
    LOG_DEBUG << "Refilter list";
    filtered_tab::filter_changed();

    tree_->setUpdatesEnabled(false);
    int visible_count = 0;
    for (int i = 0; i < tree_->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tree_->topLevelItem(i);
        server_id id = item->data(0, c_id_role).value<server_id>();
        if (id.is_empty())
            return;

        bool is_visible = true;
        server_info_p si = server_list()->get(id);
        if (si)
            is_visible = filtrate(*(si.get()));
        item->setHidden(!is_visible);
        if (is_visible)
            visible_count++;
    }
    set_visible_count(visible_count);
    tree_->setUpdatesEnabled(true);
}

server_id server_list_common_tab::selected_server() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
        return item->data(0, c_id_role).value<server_id>();
    else
        return server_id();
}

server_id_list server_list_common_tab::selection() const
{
    server_id_list res;
    foreach (QTreeWidgetItem* item, tree_->selectedItems())
    {
        res.push_back(item->data(0, c_id_role).value<server_id>());
    }
    return res;
}

void server_list_common_tab::save_state()
{
    filtered_tab::save_state();
    sls_.save_header_state(tree_->header()->saveState());
}

void server_list_common_tab::load_state()
{
    filtered_tab::load_state();
    tree_->header()->restoreState(sls_.header_state());
}

void server_list_common_tab::refresh_selected()
{
    context().job_que()->add_job(job_p(
            new job_update_selected(selection(),
                                    context().serv_list(),
                                    *context().geo())));
}

void server_list_common_tab::set_visible_count(int val)
{
    if (visible_count_ == val)
        return;
    visible_count_ = val;
    update_caption();
}

void server_list_common_tab::set_total_count(int val)
{
    if (total_count_ == val)
        return;
    total_count_ = val;
    update_caption();
}

void server_list_common_tab::update_caption()
{
    QString cnt;
    if (total_count_ > 0)
    {
        if (visible_count_ == total_count_)
            cnt = QString(" (%1)").arg(visible_count_);
        else
            cnt = QString(" (%1/%2)").arg(visible_count_).arg(total_count_);
    }

    setWindowTitle(QString("%1%2").arg(caption_).arg(cnt));
}

void server_list_common_tab::do_selection_change()
{
    emit selection_changed();
}

////////////////////////////////////////////////////////////////////////////////
// server_list_common_tab_settings

server_list_common_tab_settings::server_list_common_tab_settings(const tab_settings_p& ts)
{
    base_settings set;
    
    uid_ = ts->uid() + "_server_list_common";
    
    set.register_sub_group( uid_, "server_list_common", ts->uid() );
    sls = base_settings().get_settings(uid_);

    //TODO backward config compatibility - remove on 0.8.0
    ts_ = ts->ts();
    
    update_setting_value( ts_, sls, "header_state", "header_state" );
}

QByteArray server_list_common_tab_settings::header_state() const
{
    return sls->value("header_state").toByteArray();
}

void server_list_common_tab_settings::save_header_state(const QByteArray& ba)
{
    sls->setValue("header_state", QVariant::fromValue(ba));
}

