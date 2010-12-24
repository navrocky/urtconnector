
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

#ifndef PREF_KPAGE_VIEW_H
#define PREF_KPAGE_VIEW_H

#include <kpagedialog.h>

#include "base_view.h"

namespace detail {

struct kpage_view : base_view {
    QWidget* tw;

    kpage_view( QWidget* t, detail::connector* c )
        : tw(t)
    {
        init_ui();
        
        QObject::connect(
            kpage,  SIGNAL( currentPageChanged ( KPageWidgetItem *, KPageWidgetItem * ) ),
            c,      SLOT  ( currentPageChanged ( KPageWidgetItem *, KPageWidgetItem * ) )
        );

        QObject::connect(
            kpage,  SIGNAL( buttonClicked( KDialog::ButtonCode ) ),
            c,      SLOT  ( buttonClicked( KDialog::ButtonCode ) )
        );
    }

    virtual void insert_item(  preferences_item item, preferences_item parent  ) {
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
            kpage->addSubPage( parent_item, page_item );
        }
        else
        {
            kpage->addPage( page_item );
        }

//         kpage->setMinimumSize( kpage->sizeHint() += QSize(0,50) );
    }

    virtual preferences_widget* current_widget() const
    {
        return static_cast<preferences_widget*>( kpage->currentPage()->widget() );
    }

    virtual void set_current_widget( preferences_item item )
    {
        kpage->setCurrentPage( boost::get<KPageWidgetItem*>( item.list_item() ) );
    }

    virtual void set_current_item( preferences_item item )
    {
        kpage->setCurrentPage( boost::get<KPageWidgetItem*>( item.list_item() ) );
    }

    KPageDialog* get_kpage(){ return kpage; }

protected:
    virtual void init_ui() {
        QBoxLayout* main_box = new QBoxLayout( QBoxLayout::LeftToRight, tw );
        main_box->setContentsMargins( margins() );
        main_box->setSizeConstraint( QLayout::SetMinimumSize );
        
        kpage = new KPageDialog;
        kpage->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Default );
        kpage->setDefaultButton (KDialog::Ok );

        main_box->addWidget( kpage );
    }

private:
    KPageDialog*    kpage;
};

} //namespace detail

#endif

