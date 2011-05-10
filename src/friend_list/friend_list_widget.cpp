#include "friend_list_widget.h"

#include <boost/bind.hpp>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMap>
#include <QAction>
#include <QPixmap>
#include <QPainter>
#include <QIcon>
#include <QStyledItemDelegate>
#include <QScrollBar>

#include <common/tools.h>
#include <common/qt_syslog.h>
#include <common/tree_smart_updater.h>
#include <common/qaccumulatingconnection.h>
#include <common/server_info.h>
#include <common/server_list.h>
#include <common/player_info.h>
#include <common/item_view_dblclick_action_link.h>
#include <tabs/common_item_tags.h>
#include <tabs/visible_updater.h>
#include <jobs/job_queue.h>
#include "../job_update_selected.h"
#include "../job_update_from_master.h"
#include "friend_list.h"
#include "friend_prop_dialog.h"
#include "friend_list_db_saver.h"
#include <tabs/status_item_delegate.h>
#include <tabs/common_item_tags.h>
#include <tabs/tools.h>

Q_DECLARE_METATYPE(friend_record)
Q_DECLARE_METATYPE(QAbstractItemDelegate*)

SYSLOG_MODULE(friend_list_widget)

class proxy_item_delegate : public QStyledItemDelegate
{
public:
    proxy_item_delegate(QObject* parent)
    : QStyledItemDelegate(parent)
    {
    }

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const
    {
        QAbstractItemDelegate* d = index.data(c_delegate_ptr_role).value<QAbstractItemDelegate*>();
        if (d)
            d->paint(painter, option, index);
        else
            QStyledItemDelegate::paint(painter, option, index);
    }

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
    {
        QAbstractItemDelegate* d = index.data(c_delegate_ptr_role).value<QAbstractItemDelegate*>();
        return (d)
            ? d->sizeHint(option, index)
            : QStyledItemDelegate::sizeHint(option, index);
    }

    static void set_delegate(QTreeWidgetItem* item, int column, QAbstractItemDelegate* d)
    {
        item->setData(column, c_delegate_ptr_role, QVariant::fromValue(d));
    }
};

class name_delegate : public QStyledItemDelegate
{
public:
    name_delegate(QObject* parent, QTreeWidget* tw)
    : QStyledItemDelegate(parent)
    , tw_(tw)
    {
    }

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
                       const QModelIndex& index) const
    {
        static int padding = option.rect.left();

        int cols_width = 0;
        for (int i = 0; i < tw_->columnCount(); i++)
            cols_width += tw_->header()->sectionSize(i);

        int d = -option.rect.left() + padding - tw_->horizontalScrollBar()->sliderPosition();

        QPixmap pm(option.rect.size());
        pm.fill(Qt::transparent);
        QWidget* w = dynamic_cast<QWidget*>(painter->device());

        QPainter p(&pm);
        p.translate(d, 0);
        QStyleOptionViewItem opt = option;
        opt.rect.moveTo(0, 0);

        QFont f = opt.font;
//        f.setBold(true);
        opt.font = f;
        opt.rect.setRight(cols_width - padding - 1);

        painter->save();
        painter->translate(d, 0);
        QStyledItemDelegate::paint(&p, opt, index.model()->sibling(index.row(), 0, index));
        painter->restore();

        painter->drawPixmap(option.rect.topLeft(), pm);
    }

    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const
    {
        return QStyledItemDelegate::sizeHint(option, index);
    }

private:
    QTreeWidget* tw_;
};

friend_list_widget::friend_list_widget(friend_list* fl, const tab_context& ctx, QWidget *parent)
: main_tab(tab_settings_p(new tab_settings("friend_list")), ctx, parent)
, friends_(fl)
, caption_(this, tr("Friends"))
, updater_(new visible_updater(this, SLOT(update_contents()), this))
, online_count_(0)
{
    setWindowIcon(QIcon("icons:friends.png"));
    
    add_action_ = new QAction(QIcon("icons:add.png"), tr("Add new friend"), this);
    connect(add_action_, SIGNAL(triggered()), SLOT(add()));

    edit_action_ = new QAction(QIcon("icons:edit.png"), tr("Edit selected friend"), this);
    connect(edit_action_, SIGNAL(triggered()), SLOT(edit_selected()));

    remove_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove selected"), this);
    connect(remove_action_, SIGNAL(triggered()), SLOT(remove_selected()));

    update_selected_action_ = new QAction(QIcon("icons:view-refresh.png"), tr("Update selected server"), this);
    connect(update_selected_action_, SIGNAL(triggered()), SLOT(update_selected()));

    update_bookmarks_action_ = new QAction(QIcon("icons:bookmarks-refresh.png"), tr("Update bookmarks"), this);
    connect(update_bookmarks_action_, SIGNAL(triggered()), SLOT(update_bookmarks()));

    update_all_action_ = new QAction(QIcon("icons:download.png"), tr("Update all servers"), this);
    connect(update_all_action_, SIGNAL(triggered()), SLOT(update_all()));
    
    addAction(add_action_);
    addAction(edit_action_);
    addAction(remove_action_);
    add_separator_action(this);
    addAction(update_selected_action_);
    addAction(update_bookmarks_action_);
    addAction(update_all_action_);
    
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
//     tree_->setRootIsDecorated(false);
//     tree_->setUniformRowHeights(true);
    tree_->setSortingEnabled(true);
    tree_->setAllColumnsShowFocus(true);
//     tree_->setWordWrap(true);

    tree_->setItemDelegate(new proxy_item_delegate(this));

    status_delegate_ = new status_item_delegate(ctx.serv_list(), this);
    name_delegate_ = new name_delegate(this, tree_);
    
    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->addAction(ctx.connect_action());
    add_separator_action(tree_);
    tree_->addAction(add_action_);
    tree_->addAction(edit_action_);
    tree_->addAction(remove_action_);
    add_separator_action(tree_);
    tree_->addAction(update_selected_action_);
    tree_->addAction(update_bookmarks_action_);
    tree_->addAction(update_all_action_);

    new item_view_dblclick_action_link(this, tree_, ctx.connect_action());

    QTreeWidgetItem* it = tree_->headerItem();
    it->setText(7, tr("Address"));
    it->setText(6, tr("Players"));
    it->setText(5, tr("Map"));
    it->setText(4, tr("Game mode"));
    it->setText(3, tr("Ping"));
    it->setText(2, tr("Country"));
    it->setText(1, tr("Name"));
    it->setText(0, tr("Friend / Status"));

    QHeaderView* hdr = tree_->header();
    hdr->resizeSection(0, 80);
    hdr->resizeSection(1, 350);
    hdr->resizeSection(2, 50);
    hdr->resizeSection(3, 50);
    hdr->resizeSection(4, 60);
    hdr->setSortIndicator(3, Qt::AscendingOrder);
    
    connect(friends_, SIGNAL(changed()), SLOT(update_contents()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(update_actions()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SIGNAL(selection_changed()));
    
    // create list saver
    new friend_list_db_saver(friends_, this);
    
    new QAccumulatingConnection(context().serv_list().get(), SIGNAL(changed()),
                                updater_, SLOT(update_contents()), 200,
                                QAccumulatingConnection::Periodically,
                                this);

    updater_->update_contents();
}

server_id friend_list_widget::selected_server() const
{
    const server_set_t& ss = get_selected_servers();
    if (ss.begin() != ss.end())
        return *(ss.begin());
    else
        return server_id();
}

void friend_list_widget::update_contents()
{
    online_count_ = 0;
    const friend_list::friend_records_t& fl = friends_->list();
    
    smart_update_tree_contents(fl, c_friend_role, tree_, 0,
        boost::bind(&friend_list_widget::update_friend_item, this, _1, _2), items_map_ );


    
    caption_.set_visible_count(online_count_);
    caption_.set_total_count(tree_->topLevelItemCount());

    update_actions();
}

void friend_list_widget::update_actions()
{
    edit_action_->setEnabled(!(get_selected_friend().is_empty()));
    remove_action_->setEnabled(tree_->selectedItems().count() > 0);

    const server_set_t& ss = get_selected_servers();
    update_selected_action_->setEnabled(ss.size() > 0);
}

server_id_list friend_list_widget::find_server_with_player(const friend_record& fr)
{
    server_id_list res;
    if (fr.expression().isEmpty())
    {
        foreach (server_info_list::const_reference r, context().serv_list()->list())
        {
            foreach (const player_info& pi, r.second->players)
            {
                if (fr.nick_name().compare(pi.nick_name().trimmed(), Qt::CaseInsensitive) == 0)
                    res.append(r.second->id);
            }
        }
    } else
    {
        QRegExp rx(fr.expression());
        rx.setCaseSensitivity(Qt::CaseInsensitive);
        foreach (server_info_list::const_reference r, context().serv_list()->list())
        {
            foreach (const player_info& pi, r.second->players)
            {
                if ( rx.indexIn(pi.nick_name()) >= 0)
                    res.append(r.second->id);
            }
        }
    }
    return res;
}

void friend_list_widget::update_friend_item(QTreeWidgetItem* item, const friend_record& fr)
{
    item->setText(0, fr.nick_name());

    for (int i = 0; i < 8; i++)
        proxy_item_delegate::set_delegate(item, i, name_delegate_);

    server_id_list ids = find_server_with_player(fr);
    online_count_ += ids.size();
    
    int old_cnt = item->childCount();
    
    // take old items list
    typedef updater_traits<server_id>::ItemsByElement srv_items_map_t;
    
    srv_items_map_t items;
    for (int i = 0; i < item->childCount(); i++)
    {
        QTreeWidgetItem* it = item->child(i);
        items[it->data(0, c_id_role).value<server_id>()] = it;
    }
    
    // update items
    smart_update_tree_contents( ids, c_id_role, tree_, item, 
        boost::bind(&friend_list_widget::update_server_item, this, _1, _2), items);
    
    if (old_cnt == 0 && item->childCount() > 0)
        item->setExpanded(true);        
}

void friend_list_widget::update_server_item(QTreeWidgetItem* item, const server_id& id)
{
//    item->setText(1, id.address());
    proxy_item_delegate::set_delegate(item, 0, status_delegate_);
    update_server_info_item(context(), item);
}

void friend_list_widget::add()
{
    friend_prop_dialog d(this);
    if (d.exec() == QDialog::Rejected)
        return;
    friends_->add(d.rec());
}

friend_record friend_list_widget::get_selected_friend() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
        return item->data(0, c_friend_role).value<friend_record>();
    else
        return friend_record();
}

void friend_list_widget::edit_selected()
{
    const friend_record& fr = get_selected_friend();
    QString old_nick_name = fr.nick_name();
    friend_prop_dialog d(this);
    d.set_rec(fr);
    if (d.exec() == QDialog::Rejected)
        return;
    friends_->change(old_nick_name, d.rec());
}

void friend_list_widget::remove_selected()
{
    QList<QString> nn;
    for (int i = 0; i < tree_->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* ti = tree_->topLevelItem(i);
        if (ti->isSelected())
        {
            const friend_record& fr = ti->data(0, c_friend_role).value<friend_record>();
            nn.append(fr.nick_name());
        }
    }
    friends_->remove(nn);
}

friend_list_widget::server_set_t friend_list_widget::get_selected_servers() const
{
    server_set_t res;
    foreach (QTreeWidgetItem* item, tree_->selectedItems())
    {
        server_id id = item->data(0, c_id_role).value<server_id>();
        if (id.is_empty() && item->childCount() > 0)
            id = item->child(0)->data(0, c_id_role).value<server_id>();
        if (!id.is_empty())
            res.insert(id);
    }
    return res;
}

void friend_list_widget::update_selected()
{
    LOG_DEBUG << "Refresh selected servers";

    server_id_list l;
    const server_set_t& ss = get_selected_servers();
    foreach (const server_id& id, ss)
    {
        l.push_back(id);
    }

    context().job_que()->add_job(job_p(new job_update_selected(l,
        context().serv_list(), *context().geo())));
}

void friend_list_widget::update_bookmarks()
{
    LOG_DEBUG << "Refresh bookmarks";
    server_id_list l;

    foreach(const server_bookmark& bm, context().bookmarks()->list())
    {
        l.push_back(bm.id());
    }

    context().job_que()->add_job(job_p(new job_update_selected(l,
        context().serv_list(), *context().geo())));
}

void friend_list_widget::update_all()
{
    LOG_DEBUG << "Refresh all servers";
    context().job_que()->add_job(job_p(
        new job_update_from_master(context().serv_list(), *context().geo())));
}

