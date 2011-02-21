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
: filtered_tab(tab_settings_p(new server_list_common_tab_settings(object_name)),
               ctx, parent)
, caption_(caption)
, visible_count_(0)
, total_count_(0)
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

    QTreeWidgetItem *hi = tree_->headerItem();
    hi->setText(7, tr("Players"));
    hi->setText(6, tr("Map"));
    hi->setText(5, tr("Game mode"));
    hi->setText(4, tr("Ping"));
    hi->setText(3, tr("Country"));
    hi->setText(2, tr("Address"));
    hi->setText(1, tr("Name"));
    hi->setText(0, tr("Status"));
    hi->setToolTip(2, tr("Server address (ip:port)"));
    hi->setToolTip(1, tr("Server name"));
    hi->setToolTip(0, tr("Server status"));

    tree_->setItemDelegateForColumn( 0, new status_item_delegate( server_list(), this) );

    QHeaderView* hdr = tree_->header();

    hdr->moveSection(2, 7);
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 50);
    hdr->resizeSection(7, 60);
    hdr->setSortIndicator(4, Qt::AscendingOrder);

    update_caption();
}

void server_list_common_tab::filter_changed()
{
    LOG_DEBUG << "Refilter list";
    filtered_tab::filter_changed();

//    tree_->setUpdatesEnabled(false);
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
//    tree_->setUpdatesEnabled(true);
}

void server_list_common_tab::update_item(QTreeWidgetItem* item)
{
    server_id id = item->data(0, c_id_role).value<server_id>();
    if (id.is_empty())
        return;

    static const server_info_p empty( new server_info );

    server_info_p si = server_list()->get(id);
    if (!si)
        si = empty;

    int stamp = item->data(0, c_stamp_role).value<int>();
    if ( si->update_stamp() != stamp || stamp == 0 )
    {
        QString name = si->name;
        const server_bookmark& bm = context().bookmarks()->get(id);
        if (!bm.is_empty())
        {
            if (!bm.name().isEmpty() && name != bm.name())
            {
                if (name.isEmpty())
                    name = bm.name();
                else
                    name = QString("%1 (%2)").arg(name).arg(bm.name());
            }
        }

        QStringList sl;
        sl << si->status_name();

        if (si->is_password_needed())
            sl << tr("Private");
        if (si->get_info("pure", "-1").toInt() == 0)
            sl << tr("Not pure");

        QString status = sl.join(", ");

        item->setToolTip(0, status);
//        item->setIcon(0, QIcon("icons:status-none.png"));
        item->setText(1, name);
        item->setText(2, id.address());
        item->setIcon(3, geoip::get_flag_by_country(si->country_code));
        item->setToolTip(3, si->country);
        item->setText(4, QString("%1").arg(si->ping, 5));
        item->setText(5, si->mode_name());
        item->setText(6, si->map);

        QString player_count;
        if (si->max_player_count > 0)
            player_count = QString("%1/%2/%3").arg(si->players.size())
            .arg(si->public_slots()).arg(si->max_player_count);

        item->setText(7, player_count);
        item->setToolTip(7, tr("Current %1 / Public slots %2 / Total %3")
                         .arg(si->players.size()).arg(si->public_slots())
                         .arg(si->max_player_count));
        item->setData(0, c_stamp_role, QVariant::fromValue(si->update_stamp()));
    }
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
    server_list_common_tab_settings* s =
            static_cast<server_list_common_tab_settings*>(settings().get());
    s->save_header_state(tree_->header()->saveState());
}

void server_list_common_tab::load_state()
{
    filtered_tab::load_state();
    server_list_common_tab_settings* s =
            static_cast<server_list_common_tab_settings*>(settings().get());
    tree_->header()->restoreState(s->header_state());
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

////////////////////////////////////////////////////////////////////////////////
// server_list_common_tab_settings

server_list_common_tab_settings::server_list_common_tab_settings(const QString& object_name)
: filtered_tab_settings(object_name)
{
}

QByteArray server_list_common_tab_settings::header_state() const
{
    return st->value("header_state").toByteArray();
}

void server_list_common_tab_settings::save_header_state(const QByteArray& ba)
{
    st->setValue("header_state", QVariant::fromValue(ba));
}

