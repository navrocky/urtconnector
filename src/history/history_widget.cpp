#include "history_widget.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QAction>

#include <boost/bind.hpp>

#include <common/qt_syslog.h>
#include <common/server_list.h>
#include <common/item_view_dblclick_action_link.h>
#include <common/tools.h>
#include <common/tree_smart_updater.h>

#include <tabs/status_item_delegate.h>
#include <tabs/common_item_tags.h>
#include <tabs/status_item_delegate.h>
#include "../server_options_dialog.h"
#include "history.h"

// needed for status_item_delegate
#include "server_list_widget.h"

SYSLOG_MODULE(history_widget)

Q_DECLARE_METATYPE(history_item);

const int c_history_role = Qt::UserRole + 12;

// tree item needed to custom sort by date
class history_tree_item : public QTreeWidgetItem
{
public:
    history_tree_item(QTreeWidget *view)
    : QTreeWidgetItem(view)
    {}

    bool operator<(const QTreeWidgetItem & other) const
    {
        if (treeWidget()->sortColumn() == 2)
        {
            const history_item& hi1 = data(0, c_history_role).value<history_item>();
            const history_item& hi2 = other.data(0, c_history_role).value<history_item>();
            return hi1.date_time() < hi2.date_time();
        } else
            return QTreeWidgetItem::operator<(other);
    }
};

struct history_adapter
{
    template <typename Item>
    QTreeWidgetItem* create_item(QTreeWidget* tree, QTreeWidgetItem* parent_item,
        const Item& item, int role) const
    {
        QTreeWidgetItem* res = new history_tree_item(tree);
        res->setData(0, role, QVariant::fromValue(item));
        return res;
    }

    void remove_item(QTreeWidgetItem* item) const
    {
        delete item;
    }
};

////////////////////////////////////////////////////////////////////////////////
// history_widget

history_widget::history_widget(history_p history,
                               const tab_context& ctx,
                               QWidget *parent)
: filtered_tab(tab_settings_p(new filtered_tab_settings("history_tab")), ctx, parent)
, history_(history)
, group_mode_(true)
, item_count_(0)
, visible_item_count_(0)
, update_contents_pended_(false)
, caption_(this, tr("History"))
{
    setWindowIcon(QIcon("icons:history.png"));
    
    add_bookmark_action_ = new QAction(QIcon("icons:bookmarks.png"), tr("Add to favorites"), this);
    add_bookmark_action_->setToolTip(tr("Add selected server to favorites list"));
    connect(add_bookmark_action_, SIGNAL(triggered()), SLOT(add_to_favorites()));

    remove_selected_action_ = new QAction(QIcon("icons:remove.png"), tr("Remove selected record"), this);
    connect(remove_selected_action_, SIGNAL(triggered()), SLOT(delete_selected()));

    remove_all_action_ = new QAction(QIcon("icons:edit-clear.png"), tr("Clear history"), this);
    connect(remove_all_action_, SIGNAL(triggered()), SLOT(clear_all()));
    
    tree_ = new QTreeWidget(this);
    setCentralWidget(tree_);

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
    hdr->resizeSection(2, 150);
    hdr->resizeSection(3, 200);
    
    tree_->setItemDelegateForColumn(1, new status_item_delegate(server_list(), tree_));
    connect(history.get(), SIGNAL(changed()), SLOT(update_contents()));
    connect(tree_, SIGNAL(itemSelectionChanged()), SLOT(do_selection_change()));

    addAction(add_bookmark_action_);
    add_separator_action(this);
    addAction(remove_selected_action_);
    addAction(remove_all_action_);

    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree_->addAction(context().connect_action());
    tree_->addAction(add_bookmark_action_);
    add_separator_action(tree_);
    tree_->addAction(remove_selected_action_);
    tree_->addAction(remove_all_action_);

    new item_view_dblclick_action_link(this, tree_, ctx.connect_action());

    tree_->setSortingEnabled(true);
    tree_->sortByColumn(2, Qt::DescendingOrder);
    tree_->setAlternatingRowColors(true);
    tree_->setSelectionMode(QAbstractItemView::ExtendedSelection);
    tree_->setUniformRowHeights(true);
    tree_->setAllColumnsShowFocus(true);
    tree_->setWordWrap(true);

    update_contents();
}

void history_widget::set_group_mode(bool val)
{
    if (group_mode_ == val)
        return;
    group_mode_ = val;
    tree_->clear();
    items_map_.clear();
    update_contents();
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

void history_widget::add_to_favorites()
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

void history_widget::update_item(QTreeWidgetItem* item)
{
    const history_item& hi = item->data(0, c_history_role).value<history_item>();
    item->setText(0, hi.server_name());
    item->setText(2, hi.date_time().toString(Qt::DefaultLocaleShortDate));
    item->setText(3, hi.id().address());
    item->setText(4, hi.password());
    item->setText(5, hi.player_name());
    item->setData(0, c_id_role, QVariant::fromValue(hi.id()));
    item->setData(1, c_id_role, QVariant::fromValue(hi.id()));
}

void history_widget::update_contents_simple()
{
    tree_->setRootIsDecorated(false);
    const history::history_list_t& hl = history_->list();
    updater<history::history_list_t, history_adapter>::update_tree_contents(hl, c_history_role, tree_, 0,
        boost::bind(&history_widget::update_item, this, _1), items_map_);
}

void history_widget::update_contents_grouped()
{
    tree_->setRootIsDecorated(true);
    const history::history_list_t& hl = history_->list();
    updater<history::history_list_t, history_adapter>::update_tree_contents(hl, c_history_role, tree_, 0,
        boost::bind(&history_widget::update_item, this, _1), items_map_);
    
    // find top items
    typedef QMap<server_id, QTreeWidgetItem*> top_items_t;
    top_items_t top_items;
    items_map_t::iterator it = items_map_.begin();
    for (; it != items_map_.end(); ++it)
    {
        const history_item& hi = it.key();
        
        top_items_t::iterator i = top_items.find(hi.id());
        if (i == top_items.end())
        {
            top_items[hi.id()] = it.value();
        } else
        {
            const history_item& hi2 = i.value()->data(0, c_history_role).value<history_item>();
            if (hi.date_time() > hi2.date_time())
                top_items[hi.id()] = it.value();
        }
    }
    
    // arrange top items
    foreach (QTreeWidgetItem* item, top_items)
    {
        if (item->parent())
            tree_->addTopLevelItem(item);
    }
    
    // arrange nested items
    it = items_map_.begin();
    for (; it != items_map_.end(); ++it)
    {
        QTreeWidgetItem* item = it.value();
        const history_item& hi = it.key();
        top_items_t::iterator i = top_items.find(hi.id());
        QTreeWidgetItem* top_item = i.value();
        
        if (item != top_item && item->parent() != top_item)
        {
            if (item->parent())
                item->parent()->takeChild(item->parent()->indexOfChild(item));
            else
                tree_->takeTopLevelItem(tree_->indexOfTopLevelItem(item));
            top_item->addChild(item);
        }
    }
}

void history_widget::update_contents()
{
    if (!isVisible())
    {
        update_contents_pended_ = true;
        return;
    }

    LOG_DEBUG << "Update contents";

    tree_->setUpdatesEnabled(false);
    tree_->setSortingEnabled(false);

    if (group_mode_)
        update_contents_grouped();
    else
        update_contents_simple();

    tree_->setSortingEnabled(true);
    tree_->setUpdatesEnabled(true);
    
    caption_.set_total_count(items_map_.size());

    filter_changed();
    update_actions();
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
    
    int visible_count = 0;
    
    foreach (QTreeWidgetItem* item, items_map_)
    {
        bool is_visible = true;
        if (server_list())
        {
            server_info_p si = server_list()->get(item->data(0, c_id_role).value<server_id > ());
            if (si)
                is_visible = filtrate(*si);
        }
        
        if (is_visible)
            visible_count++;

        if (item->isHidden() != !is_visible)
            item->setHidden(!is_visible);
    }
    caption_.set_visible_count(visible_count);
}

void history_widget::do_selection_change()
{
    emit selection_changed();
    update_actions();
}

void history_widget::update_actions()
{
    add_bookmark_action_->setEnabled(!selected_server().is_empty());
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

