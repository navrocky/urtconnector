#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QAction>

#include <boost/bind.hpp>

#include <common/qt_syslog.h>
#include <common/server_list.h>
#include <common/item_view_dblclick_action_link.h>
#include <common/tools.h>
#include <tabs/status_item_delegate.h>
#include <tabs/common_item_tags.h>
#include <tabs/status_item_delegate.h>
#include "history.h"

// needed for status_item_delegate
#include "server_list_widget.h"

#include "history_widget.h"

SYSLOG_MODULE(history_widget)

Q_DECLARE_METATYPE(history_item);

const int c_history_role = Qt::UserRole + 12;

history_widget::history_widget(history_p history,
                               const tab_context& ctx,
                               QWidget *parent)
: filtered_tab(tab_settings_p(new filtered_tab_settings("history_tab")), ctx, parent)
, history_(history)
, group_mode_(true)
, item_count_(0)
, visible_item_count_(0)
, update_contents_pended_(false)
{
    setWindowTitle(tr("History"));
    setWindowIcon(QIcon("icons:history.png"));

    remove_selected_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove selecter record"), this);
    connect(remove_selected_action_, SIGNAL(triggered()), SLOT(delete_selected()));

    remove_all_action_ = new QAction(QIcon("icons:edit-clear.png"), tr("Clear history"), this);
    connect(remove_all_action_, SIGNAL(triggered()), SLOT(clear_all()));
    
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);
//     connect(p_->ui.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));

    QTreeWidgetItem* it = tree_->headerItem();
    it->setText(0, tr("Server name"));
    it->setText(1, tr("Status"));
    it->setText(2, tr("Date and time"));
    it->setText(3, tr("Address"));
    it->setText(4, tr("Password"));
    it->setText(5, tr("Player name"));

    QHeaderView* hdr = tree_->header();
    hdr->resizeSection(0, 300);
    hdr->resizeSection(1, 80);
    hdr->resizeSection(2, 200);

    tree_->setItemDelegateForColumn(1, new status_item_delegate(server_list(), tree_));
    connect(history.get(), SIGNAL(changed()), SLOT(update_contents()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(do_selection_change()));

    addAction(remove_selected_action_);

    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->addAction(context().connect_action());
    add_separator_action(tree_);
    tree_->addAction(remove_selected_action_);

    new item_view_dblclick_action_link(this, tree_, ctx.connect_action());

    update_contents();
}

void history_widget::set_group_mode(bool val)
{
    if (group_mode_ == val)
        return;
    group_mode_ = val;
    tree_->clear();
    update_contents();
}

void history_widget::update_caption()
{
    QString num;
    if (item_count_ > 0)
        num = QString(" (%1)").arg(item_count_);
    setWindowTitle(tr("History%1").arg(num));
}

void history_widget::showEvent(QShowEvent* event)
{
    filtered_tab::showEvent(event);
    if (update_contents_pended_)
    {
        update_contents_pended_ = false;
        update_contents();
    }
}

template <typename Item>
QTreeWidgetItem* find_item(QTreeWidget* tree, QTreeWidgetItem* parent_item,
                           int role, const Item& item)
{
    QTreeWidgetItem* res = 0;
    if (parent_item)
    {
        for (int i = 0; i < parent_item->childCount(); ++i)
        {
            res = parent_item->child(i);
            const Item& it = res->data(0, role).value<Item>();
            if (it == item)
                return res;
        }
    } else
    {
        for (int i = 0; i < tree->topLevelItemCount(); ++i)
        {
            res = tree->topLevelItem(i);
            const Item& it = res->data(0, role).value<Item>();
            if (it == item)
                return res;
        }
    }
    return 0;
}

struct std_adapter
{
    template <typename Item>
    QTreeWidgetItem* create_item(QTreeWidget* tree, QTreeWidgetItem* parent_item,
        const Item& item, int role) const
    {
        QTreeWidgetItem* res;
        if (parent_item)
            res = new QTreeWidgetItem(parent_item);
        else
            res = new QTreeWidgetItem(tree);
        res->setData(0, role, QVariant::fromValue(item));
        return res;
    }

    void remove_item(QTreeWidgetItem* item) const
    {
        delete item;
    }
};

template <typename List, typename Adapter>
void update_tree_contents(const List& l, 
                          int role,
                          QTreeWidget* tree,
                          QTreeWidgetItem* parent_item, 
                          const boost::function<void (QTreeWidgetItem*)>& update_item,
                          const Adapter& adapter)
{
    // who appeared ?
    foreach (typename List::const_reference item, l)
    {
        // search item
        QTreeWidgetItem* it = find_item(tree, parent_item, role, item);
        if (!it)
            it = adapter.create_item(tree, parent_item, item, role);
        update_item(it);
    }

    // remove old items


}

void history_widget::update_item(QTreeWidgetItem*)
{
    
}


void history_widget::update_contents_simple()
{
    const history::history_list_t& hl = history_->list();
    std_adapter a;
    update_tree_contents(hl, c_history_role, tree_, 0,
        boost::bind(&history_widget::update_item, this, _1), a);

//    history::history_list_t::const_reference



//    const server_bookmark_list::bookmark_map_t& bms = context().bookmarks()->list();

//    // who changed, appeared?
//    foreach (const history_item& bm, hl)
//    {
//        const server_id& id = bm.id();
//        server_items::iterator it2 = items_.find(id);
//        QTreeWidgetItem* item;
//
//        if (it2 == items_.end())
//        {
//            item = new QTreeWidgetItem( tree() );
//            item->setData( 0, c_id_role, QVariant::fromValue(id) );
//            items_[id] = item;
//        }
//    }
//
//    // remove old items
//    QList<server_id> to_remove;
//    for (server_items::iterator it = items_.begin(); it != items_.end(); it++)
//    {
//        const server_id& id = it->first;
//        if ( bms.find(id) != bms.end() )
//            continue;
//        QTreeWidgetItem* item = it->second;
//        delete item;
//        items_.erase(it);
//    }
//
//    update_servers_info();
}

void history_widget::update_contents_grouped()
{
}

void history_widget::update_contents()
{
    if (!isVisible())
    {
        update_contents_pended_ = true;
        return;
    }

    LOG_DEBUG << "Update contents";

//    tree()->setUpdatesEnabled(false);
//    tree()->setSortingEnabled(false);

    // take id
//    if (group_mode_)
//        update_contents_grouped();
//    else
//        update_contents_simple();



//    tree()->setSortingEnabled(true);
//    tree()->setUpdatesEnabled(true);








    tree_->clear();
    item_count_ = 0;

    foreach (const history_item& item, history_->list())
    {
        addItem(item);
        item_count_++;
    }

    filter_changed();
    update_actions();
}

void history_widget::addItem(const history_item& item)
{
    QTreeWidgetItem* item_ptr = new QTreeWidgetItem();

    item_ptr->setText(0, item.server_name());
    item_ptr->setText(2, item.date_time().toString(Qt::DefaultLocaleShortDate));
    item_ptr->setText(3, item.id().address());
    item_ptr->setText(4, item.password());
    item_ptr->setText(5, item.player_name());
    item_ptr->setData(0, c_id_role, QVariant::fromValue(item.id()));
    item_ptr->setData(1, c_id_role, QVariant::fromValue(item.id()));
    item_ptr->setData(0, c_history_role, QVariant::fromValue(item));

    if (QTreeWidgetItem * parent = add_item(item_ptr))
        resort(parent);
}

QTreeWidgetItem* history_widget::add_item(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parent = find_item(item->data(0, c_id_role).value<server_id > ());

    if (parent)
    {
        item->setData(1, c_suppress_role, true);
        parent->insertChild(0, item);
        return parent;
    }
    else
    {
        item->setData(1, c_suppress_role, false);
        tree_->insertTopLevelItem(0, item);
        return 0;
    }
}

server_id history_widget::selected_server() const
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (item)
        return item->data(0, c_id_role).value<server_id>();
    else
        return server_id();
}

void history_widget::filter_changed()
{
    filtered_tab::filter_changed();

    for (int i = 0; i < tree_->topLevelItemCount(); i++)
    {
        QTreeWidgetItem* item = tree_->topLevelItem(i);
        bool is_visible = true;
        if (server_list())
        {
            server_info_p si = server_list()->get(item->data(0, c_id_role).value<server_id > ());
            if (si)
                is_visible = filtrate(*si);
        }

        if (item->isHidden() != !is_visible)
            item->setHidden(!is_visible);
    }
    update_caption();
}

QTreeWidgetItem* history_widget::find_item(const server_id& id) const
{
    for (int i = 0; i < tree_->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = tree_->topLevelItem(i);
        if (item->data(0, c_id_role).value<server_id > () == id)
            return item;
    }
    return 0;
}

void history_widget::do_selection_change()
{
    emit selection_changed();
    update_actions();
}

void history_widget::resort(QTreeWidgetItem* item)
{
    int index = tree_->indexOfTopLevelItem(item);
    if (index == -1)
        return;

    item = tree_->takeTopLevelItem(index);

    QList<QTreeWidgetItem*> chlds = item->takeChildren();

    //item itself is under resorting too!
    chlds << item;

    std::sort(chlds.begin(), chlds.end(),
              boost::bind(&QTreeWidgetItem::text, _1, 0) < boost::bind(&QTreeWidgetItem::text, _2, 0));

    std::for_each(chlds.begin(), chlds.end(), boost::bind(&history_widget::add_item, this, _1));
}

void history_widget::update_actions()
{
    remove_selected_action_->setEnabled(!selected_server().is_empty());
    remove_all_action_->setEnabled(history_->list().size() > 0);
}

void history_widget::clear_all()
{
    history_->clear();
}

void history_widget::delete_selected()
{
    QList<QTreeWidgetItem*> items = tree_->selectedItems();
    foreach(QTreeWidgetItem* it, items)
    {
        const history_item& hi = it->data(0, c_history_role).value<history_item>();
        history_->remove(hi);
    }
}

