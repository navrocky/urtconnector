#ifndef TREE_SMART_UPDATER_H
#define TREE_SMART_UPDATER_H

#include <algorithm>
#include <QTreeWidgetItem>
#include <boost/function.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_facade.hpp>

//    typedef map_adapter<my_type> MT;
//    MT mt(l);
//     updater<my_type>::update_tree_contents(l, c_history_role, tree_, 0,
//         boost::bind(&history_widget::update_item, this, _1), items);
//    updater<MT>::update_tree_contents(mt, c_history_role, tree_, 0,
//        boost::bind(&history_widget::update_item, this, _1), items);

class QTreeWidget;

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
    typedef char        yes_type;
    typedef int         no_type;
    
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
            typename Container::const_iterator i = find(l, it.key());
            if (i != l.end())
                continue;
            // can't find source element - delete tree item
            QTreeWidgetItem* item = it.value();

            // reparent childs
            if (item->childCount() > 0)
            {
                if (item->parent())
                    item->parent()->addChildren(item->takeChildren());
                else
                    tree->addTopLevelItems(item->takeChildren());
            }

            delete item;
            items.erase(it);
        }
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

#endif