#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QHeaderView>
#include <QAction>

#include <algorithm>
#include <boost/bind.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_facade.hpp>

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

//template <typename Item>
//QTreeWidgetItem* find_item(QTreeWidget* tree, QTreeWidgetItem* parent_item,
//                           int role, const Item& item)
//{
//    QTreeWidgetItem* res = 0;
//    if (parent_item)
//    {
//        for (int i = 0; i < parent_item->childCount(); ++i)
//        {
//            res = parent_item->child(i);
//            const Item& it = res->data(0, role).value<Item>();
//            if (it == item)
//                return res;
//        }
//    } else
//    {
//        for (int i = 0; i < tree->topLevelItemCount(); ++i)
//        {
//            res = tree->topLevelItem(i);
//            const Item& it = res->data(0, role).value<Item>();
//            if (it == item)
//                return res;
//        }
//    }
//    return 0;
//}

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


//metaclass for checking if class has find functionm with Arg argument and Ret result type
template<typename T, typename Ret, typename Arg> struct class_has_find
{
    typedef char 	yes_type;
    typedef int 	no_type;
    
    template<typename U, Ret (U::*fn)(Arg) const> struct Checker
    { typedef yes_type result; };

    template<typename U> static typename Checker<U, &U::find>::result has_member(U*);
    static no_type has_member(...);

    static const int value = sizeof(has_member((T*)NULL)) == 1 ? 1 : 0;      
};

template <typename T>
struct get_value_type{
    typedef typename T::value_type type;
};

template <typename T, typename U>
struct get_value_type< QMap<T, U> >{
    typedef U type;
};

template <typename Container, typename Adapter = std_adapter>
struct updater
{
    typedef typename get_value_type<Container>::type Element;
    typedef QMap<Element, QTreeWidgetItem*> item_map_t;

    static void update_tree_contents(const Container& l,
                              int role,
                              QTreeWidget* tree,
                              QTreeWidgetItem* parent_item,
                              const boost::function<void (QTreeWidgetItem*)>& update_item,
                              item_map_t& items,
                              const Adapter& adapter = Adapter())
    {
        // who appeared ?

        foreach (const Element& item, l)
        {
            // search item
            typename item_map_t::iterator it = items.find(item);
            QTreeWidgetItem* ti;
            if (it == items.end())
            {
                ti = adapter.create_item(tree, parent_item, item, role);
                items[item] = ti;
            } else
                ti = it.value();
            update_item(ti);
        }

        // remove old items
        typename item_map_t::iterator it = items.begin();
        for (; it != items.end(); ++it)
        {
//            typename List::const_iterator i =
           find(l, it.key());
//            std::find( l.begin(), l.end(),  )
//            l.find(it.key());
        }
//        foreach (typename List::const_reference , )
        



    }


    // using SFINAE for handling different types of container 
    // http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error

    template <typename T>
    static typename Container::const_iterator find(
        const T& container, const Element& element,
        typename boost::disable_if< class_has_find<T, typename Container::const_iterator, const Element&> >::type* dummy = 0
    )
    {
        return std::find(container.begin(), container.end(), element);
    }

    template <typename T>
    static typename Container::const_iterator find(
        const T& container, const Element& element,
        typename boost::enable_if<class_has_find<T, typename Container::const_iterator, const Element&> >::type* dummy = 0
        )
    {
        return container.find(element);
    }
};



//struct that adapts std::map tp QMap type interfaces. class holds only refference to the std::map
template <typename Map>
struct map_adapter{
    typedef typename Map::key_type key_type;
    typedef typename Map::mapped_type mapped_type;
    
    typedef typename Map::mapped_type value_type;
    
    typedef std::map<key_type, mapped_type> RestoredMap;
    
    RestoredMap& map;
    
    map_adapter(Map& m):map(m){}
    
    template <class Type, class BaseIterator>
    class adapt_iterator
      : public boost::iterator_facade<
	    adapt_iterator<Type, BaseIterator>
	  , Type
	  , boost::forward_traversal_tag
	>
    {
      public:
	  adapt_iterator() : it() {}

	  explicit adapt_iterator(BaseIterator it) : it(it) {}

	  template <class T,class I>
	  adapt_iterator(adapt_iterator<T, I> const& other) : it(other.it) {}

      private:
	  friend class boost::iterator_core_access;
	  template <class, class> friend class adapt_iterator;

	  template <class T,class I>
	  bool equal(adapt_iterator<T, I> const& other) const
	  { return this->it == other.it; }

	  void increment()
	  { ++it; }

	  Type& dereference() const
	  { return it->second; }

	  BaseIterator it;
    };
    
    typedef adapt_iterator<value_type, typename RestoredMap::iterator > iterator;
    typedef adapt_iterator<value_type const, typename RestoredMap::const_iterator > const_iterator;
    
    iterator begin() { return iterator(map.begin()); }
    const_iterator begin() const { return const_iterator(map.begin()); }
    
    iterator end() { return iterator(map.end()); }
    const_iterator end() const { return const_iterator(map.end()); }
    
    iterator find( const key_type& k) { return iterator(map.find(k)); }
    const_iterator find( const key_type& k) const { return const_iterator(map.find(k)); }
    
};

void history_widget::update_item(QTreeWidgetItem*)
{
    
}

void history_widget::update_contents_simple()
{
    const history::history_list_t& hl = history_->list();

//    QMap<int, int>::
//    history::history_list_t::value_type
//    std_adapter a;
// 
    updater<history::history_list_t>::update_tree_contents(hl, c_history_role, tree_, 0,
        boost::bind(&history_widget::update_item, this, _1), items_map_);

//     typedef QMap<int, int> my_type;
    typedef std::map<int, int> my_type;
    typedef QMap<int, QTreeWidgetItem*> my_items;

    my_type l;
    my_items items;

    typedef map_adapter<my_type> MT;
    MT mt(l);

//     updater<my_type>::update_tree_contents(l, c_history_role, tree_, 0,
//         boost::bind(&history_widget::update_item, this, _1), items);
    
    updater<MT>::update_tree_contents(mt, c_history_role, tree_, 0,
        boost::bind(&history_widget::update_item, this, _1), items);



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
    if (group_mode_)
        update_contents_grouped();
    else
        update_contents_simple();



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

