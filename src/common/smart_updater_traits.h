
#ifndef URT_SMART_UPDATER_TRAITS_H
#define URT_SMART_UPDATER_TRAITS_H

#include <boost/function.hpp>

#include <QMap>

class QTreeWidgetItem;

namespace detail{
    

    
} // namesapce detail


///traits to handle types used in generic smart QTreeWidget updater
template <typename T>
struct updater_traits{
    
    typedef T Element;
    
    ///Associative container to link \b element with QTreeWidgetItem
    typedef QMap<Element, QTreeWidgetItem*>                     ItemsByElement;
    
    ///Type of function that handles updated QTreeWidgetItem and \b element associated with it
    typedef boost::function<void (QTreeWidgetItem*, const Element&)> Updater;
};


#endif

