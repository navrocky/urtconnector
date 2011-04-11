#ifndef TREE_SMART_UPDATER_H
#define TREE_SMART_UPDATER_H

#include <algorithm>
#include <QTreeWidgetItem>
#include <boost/utility/enable_if.hpp>
#include <boost/iterator/iterator_facade.hpp>

#include <common/smart_updater_traits.h>

class QTreeWidget;

namespace detail {

struct std_adapter
{
    QTreeWidgetItem* create_item(QTreeWidget* tree, QTreeWidgetItem* parent_item) const
    {
        return ( parent_item )
            ? new QTreeWidgetItem(parent_item)
            : new QTreeWidgetItem(tree);
    }

    void remove_item(QTreeWidgetItem* item) const
    { delete item; }
};

///metaclass for checking if class has 'find' function with \p Arg argument and \return Ret result type
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

/**
 * @brief helper to implement handling different types of container for smart updating
 * this class used SFINAE:
 * http://en.wikipedia.org/wiki/Substitution_failure_is_not_an_error
 *
 **/
template <typename Container, typename Element>
struct update_helper{
    template <typename T>
    static typename Container::const_iterator find(
        const T& container, const Element& element,
        typename boost::disable_if< detail::class_has_find<T, typename Container::const_iterator, const Element&> >::type* dummy = 0
    )
    {
        return std::find(container.begin(), container.end(), element);
    }

    template <typename T>
    static typename Container::const_iterator find(
        const T& container, const Element& element,
        typename boost::enable_if< detail::class_has_find<T, typename Container::const_iterator, const Element&> >::type* dummy = 0
        )
    {
        return container.find(element);
    }
};

template <typename T>
struct get_value_type{
    typedef typename T::value_type type;
};

template <typename T, typename U>
struct get_value_type< QMap<T, U> >{
    typedef U type;
};

template <typename Container>
struct updater_container_traits {
    typedef updater_traits< typename get_value_type<Container>::type > Traits;
    typedef typename Traits::Element         Element;
    typedef typename Traits::ItemsByElement  ItemsByElement;
    typedef typename Traits::Updater         Updater;
};

} //namespace detail



/**
 * @brief generic smart QTreeWidget updater
 *
 * @param l - container that stores \b elements that was updated( added, removed or changed )
 * @param role - Qt role-engine identifyer to assign our \b elements to QTreeWidgetItem
 * @param tree - Qt QTreeWidget
 * @param parent_item 
 * @param update_item - function that implement specific actions for item associated with updated \b elements
 * @param items - associative container to link \b element with QTreeWidgetItem
 * @param adapter custom implementation of creating and erasing QTreeWidgetItems. Defaults to Adapter().
 * @return void
 * 
 * @note see updater_traits to know type of update_item paramenter
 **/
template <typename Container, typename Adapter>
void smart_update_tree_contents(const Container& l,
                            int role,
                            QTreeWidget* tree,
                            QTreeWidgetItem* parent_item,
                            const typename detail::updater_container_traits<Container>::Updater&  updater,
                            typename detail::updater_container_traits<Container>::ItemsByElement& items,
                            const Adapter& adapter = Adapter())
{
    typedef typename detail::updater_container_traits<Container>::Element Element;
    typedef typename detail::updater_container_traits<Container>::ItemsByElement ItemsByElement;
    
    // who appeared ?
    foreach (const Element& item, l)
    {
        // search item
        typename ItemsByElement::const_iterator it = items.find(item);
        
        if ( it == items.end() )
            it = items.insert( item, adapter.create_item(tree, parent_item) );
        
        QTreeWidgetItem* ti = it.value();

        ti->setData(0, role, QVariant::fromValue(item));

        updater(ti, item);
    }

    //to avoid deletion from container when iteration over this container
    std::list<typename ItemsByElement::iterator> to_delete;

    // remove old items
    typename ItemsByElement::iterator it = items.begin();
    for (; it != items.end(); ++it)
    {
        if ( detail::update_helper<Container, Element>::find(l, it.key()) != l.end() )
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

        adapter.remove_item(item);
        to_delete.push_back(it);
    }
    
    foreach( const typename ItemsByElement::iterator& it, to_delete ) {
        items.erase(it);
    }
}

/**
 * @brief generic smart QTreeWidget updater used default adapter
 * This is overloaded function that uses detail::std::adapter as default one for smart_update_tree_contents
 **/
template <typename Container>
void smart_update_tree_contents(const Container& l,
                                int role,
                                QTreeWidget* tree,
                                QTreeWidgetItem* parent_item,
                                const typename detail::updater_container_traits<Container>::Updater& updater,
                                typename detail::updater_container_traits<Container>::ItemsByElement& items)
{
    return smart_update_tree_contents(l, role, tree, parent_item, updater, items, detail::std_adapter());
}

///struct that adapts std::map tp QMap type interfaces. class holds only refference to the std::map
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
          adapt_iterator(): it() {}

          explicit adapt_iterator(BaseIterator it) : it(it) {}

          template <class T,class I>
          adapt_iterator(adapt_iterator<T, I> const& other) : it(other.it) {}

      private:
          friend class boost::iterator_core_access;
          template <class, class> friend class adapt_iterator;

          template <class T,class I>
          bool equal(adapt_iterator<T, I> const& other) const { return this->it == other.it; }

          void increment() { ++it; }

          Type& dereference() const { return it->second; }

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