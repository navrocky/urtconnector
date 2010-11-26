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

#ifndef CONFIG_WRAPPERS_IMPL_H
#define CONFIG_WRAPPERS_IMPL_H

#include <boost/function.hpp>
#include <boost/bind.hpp>

#include <preferences_item.h>
#include <preferences_widget.h>

///Type of the function that insert a widget
typedef boost::function<void ( preferences_item ) > WidgetInserter;

///Type of the function that insert a Tree/List widget items
typedef boost::function<void ( preferences_item, preferences_item ) > ItemInserter;

///Type of the function that return a current widget
typedef boost::function<preferences_widget* () > CurrentWidgetExtractor;

///Type of the function that sets preferences_widget from preferences_item as current widget
typedef boost::function<void (preferences_item) >   CurrentWidgetSetter;

///Type of the function that sets preferences_widget from preferences_item as current widget
typedef boost::function<void (preferences_item) >   CurrentItemSetter;

namespace detail {

///struct, that incapsulates traits for different refferences representation
template <typename Form>
struct prefferences_form_traits{};


} //namespace detail

#include "plain_functors.h"
#include "tree_functors.h"
#include "tab_functors.h"
#include "list_functors.h"

#ifdef USE_KDE_DIALOG
    #include "kde_functors.h"
#endif

///helper fuction to make concrete inserter
template <typename Form>
ItemInserter make_item_inserter( Form& f )
{ return typename detail::prefferences_form_traits<Form>::item_inserter(f); }

///helper fuction to make concrete inserter
template <typename Form>
WidgetInserter make_widget_inserter( Form& f )
{ return typename detail::prefferences_form_traits<Form>::widget_inserter(f); }

///helper fuction to make concrete setter
template <typename Form>
CurrentWidgetSetter make_widget_setter( Form& f )
{ return typename detail::prefferences_form_traits<Form>::widget_setter(f); }

///helper fuction to make concrete extractor
template <typename Form>
CurrentWidgetExtractor make_widget_extractor( Form& f )
{ return typename detail::prefferences_form_traits<Form>::widget_extractor(f); }

///helper fuction to set currentItem
template <typename Form>
CurrentItemSetter make_item_setter( Form& f )
{ return typename detail::prefferences_form_traits<Form>::item_setter(f); }


#endif

