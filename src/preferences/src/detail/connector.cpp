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

#include <boost/bind.hpp>

#include <QTreeWidget>
#include <QListWidget>
#include <QDialogButtonBox>

#include "preferences_dialog.h"
#include "connector.h"

detail::connector::connector(preferences_dialog* dialog, std::list<preferences_item>& items, QDialogButtonBox* buttons)
    : QObject( dialog )
    , dialog_( dialog )
    , items_(items)
    , buttons_( buttons )
{}

detail::connector::~ connector()
{}

void detail::connector::currentItemChanged(QTreeWidgetItem * current, QTreeWidgetItem * ) 
{
    emit item_changed( current->data( 0, preferences_item::qobject_property_role ).value<preferences_item>() );
}

void detail::connector::currentItemChanged(QListWidgetItem * current, QListWidgetItem * ) 
{
    emit item_changed( current->data( preferences_item::qobject_property_role ).value<preferences_item>() );
}

void detail::connector::currentTabChanged( int new_index ) 
{
    std::list<preferences_item>::iterator it;

    it = find_if( items_.begin(), items_.end(),
            ( bind( &preferences_item::widget, _1 ) == dialog_->current_widget() ) &&
            ( bind( &preferences_item::index, _1 ) == new_index )
    );

    if ( it != items_.end() )
        emit item_changed(*it);
}

#ifdef USE_KDE_DIALOG
void detail::connector::currentPageChanged(KPageWidgetItem * current, KPageWidgetItem * ) 
{
    emit item_changed( current->property( preferences_item::qobject_property ).value<preferences_item>() );
}
#endif


void detail::connector::buttonClicked(QAbstractButton* button)
{
    emit button_clicked( buttons_->standardButton(button) );
}

#ifdef USE_KDE_DIALOG
void detail::connector::buttonClicked(KDialog::ButtonCode button)
{
    int button_code;
    switch (button)
    {
        case KDialog::Default:  button_code = QDialogButtonBox::RestoreDefaults; break;
        case KDialog::Ok:       button_code = QDialogButtonBox::Ok; break;
        case KDialog::Apply:    button_code = QDialogButtonBox::Apply; break;
        case KDialog::Cancel:   button_code = QDialogButtonBox::Cancel; break;
        default:
            button_code = QDialogButtonBox::NoButton;
    }
    emit button_clicked( button_code );
}
#endif











