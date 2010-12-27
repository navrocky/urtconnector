/***************************************************************************
 *   Copyright (C) 2010 by jerry   *
 *   jerry@jerry_work   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef CL_PREFERENCES_ITEM_H
#define CL_PREFERENCES_ITEM_H

#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#include <QVariant>

class QTreeWidgetItem;
class QListWidgetItem;

#ifdef USE_KDE_DIALOG
class KPageWidgetItem;
#endif

namespace detail {
typedef boost::variant<
    QTreeWidgetItem*,
    QListWidgetItem*
#ifdef USE_KDE_DIALOG
    ,KPageWidgetItem*
#endif
>  ListItem;
} //namespace detail

class preferences_widget;
class preferences_item;

typedef std::list<preferences_item> PreferencesItemList;

///Class thar represents an entry in preferences_dialog
class preferences_item{
public:

    static const char* qobject_property;
    static const int qobject_property_role;

    ///Type for storing id of item in list/tree, where current item inserted
    typedef detail::ListItem ListItem;

    ///En empty constructor
    preferences_item();

    ///Creates an item that owns preferences_widget and attach it to preferences_dialog
    preferences_item( preferences_widget* cw, preferences_item parent);

    ~preferences_item();

    bool operator==(const preferences_item& other) const;
    bool operator!=(const preferences_item& other) const;

    ///Is item Not empty?
    operator bool() const;

    ///Add child to the item hierarhy
    void add_child(preferences_item);

    ///Get parent of the item
    preferences_item parent() const;

    ///Get all childs of the item
    PreferencesItemList childs() const;

    ///Get preferences_widget owned by this item
    preferences_widget* widget() const;

//private:

    ///Return owning of the widget( setParent(0) and other... )
    void summon();
    
    void set_index(int index);
    int index() const;

    
    void set_list_item( const ListItem& );
    ListItem list_item() const;

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};

Q_DECLARE_METATYPE(preferences_item);

#endif

