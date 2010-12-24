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
 *  ~ GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/


#ifndef CL_PREFERENCES_H
#define CL_PREFERENCES_H

#include <list>
#include <memory>

#include <QDialog>

#include "preferences_item.h"

class QPushButton;

class preferences_widget;
class preferences_item;
class preferences_dialog;

/*! \brief Dialog that manages couple of preferences_widgets
    Provides:
        - accept/reject functionaity
        - reset to defaults
        - subdialogs
*/
class preferences_dialog: public QDialog{
    Q_OBJECT
public:

    enum Type{
        Tree,
        Tab,
        List,
        Plain,
        Auto
    };

    preferences_dialog(Type t, bool native = false, QWidget* parent = 0, Qt::WindowFlags f = 0);
    ~preferences_dialog();

    ///add item at the bottom of other items. At the bottom of childs if  \b parent not 0;
    preferences_item add_item( preferences_widget* cw, const preferences_item& parent = preferences_item() );

public Q_SLOTS:
    ///Tell to dialog that some changes done.
    void changed();
    ///Defalut QDialog accept action
    void accept();
    ///Defalut QDialog reject action
    void reject();

    ///Apply changes in current preferences_widget
    void apply_current();
    ///Reject changes in current preferences_widget
    void reject_current();

    void restoreClicked();
    void okClicked();
    void applyClicked();
    void cancelClicked();

    const preferences_item& current_item() const;
    const preferences_widget* current_widget() const;

private Q_SLOTS:

    void button_clicked( int button );
    void item_changed( const preferences_item& new_item );

private:
    Type calculate_type() const;

    void setup_ui();
    void setup_native_ui();

    void setup_clear();

    void reset();
    QPushButton* button(int button) const;

private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};





#endif

