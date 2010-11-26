
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

#ifndef CL_CONFIG_KDE_F_H
#define CL_CONFIG_KDE_F_H

#include <kpagedialog.h>

namespace detail {  namespace kde {


///Insert items to KPageDialog
struct item_inserter {
    KPageDialog& form;
    item_inserter( KPageDialog& form ) : form( form ) {}

    void operator()( preferences_item item, preferences_item parent )
    {
        preferences_widget* cw = item.widget();

        KPageWidgetItem* page_item = new KPageWidgetItem( cw );

        QIcon ic = cw->icon();
        page_item->setIcon( KIcon( ic ) );
        page_item->setName( cw->name() );
        page_item->setProperty( preferences_item::qobject_property, qVariantFromValue( item ) );

        preferences_item::ListItem varitem = page_item;

        item.set_list_item( varitem );

        if ( parent )
        {
            KPageWidgetItem* parent_item = boost::get<KPageWidgetItem*>( parent.list_item() );
            form.addSubPage( parent_item, page_item );
        }
        else
        {
            form.addPage( page_item );
        }
    }
};


///Add \b item-widget to KDE-native(KPageDialog) config UI;
struct widget_inserter {
    KPageDialog& form;
    widget_inserter( KPageDialog& form ) :form(form) {}

    void operator()(  preferences_item item )
    {}
};


///Extract current widget from KDE-native(KPageDialog) config UI;
struct widget_extractor{
    KPageDialog& form;
    widget_extractor( KPageDialog& form ) : form(form) {}

    preferences_widget* operator()()
    {
        return static_cast<preferences_widget*>( form.currentPage()->widget() );
    }
};


///Set current \b item-widget into KDE-native(KPageDialog) config UI;
struct widget_set{
    KPageDialog& form;
    widget_set( KPageDialog& form ) : form(form) {}

    void operator()( preferences_item item )
    {
        form.setCurrentPage( boost::get<KPageWidgetItem*>( item.list_item() ) );
    }
};


///Set current item into KDE-native(KPageDialog) config UI;
struct set_current_item{
    KPageDialog& form;
    set_current_item( KPageDialog& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        form.setCurrentPage( boost::get<KPageWidgetItem*>( item.list_item() ) );
    }
};

} // namespace kde


template<>
struct prefferences_form_traits<KPageDialog>
{
    typedef kde::item_inserter item_inserter;
    typedef kde::widget_inserter widget_inserter;
    typedef kde::widget_set widget_setter;
    typedef kde::widget_extractor widget_extractor;
    typedef kde::set_current_item item_setter;
};

} //namespace detail

#endif

