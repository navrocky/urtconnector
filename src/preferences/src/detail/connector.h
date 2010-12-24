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

#ifndef CONFIG_CONNECTORS_H
#define CONFIG_CONNECTORS_H

#include <QObject>
#include <QPointer>

class QTreeWidgetItem;
class QListWidgetItem;
class QDialogButtonBox;
class QAbstractButton;

#ifdef USE_KDE_DIALOG
    #include <kpagedialog.h>
#endif

class preferences_dialog;
class preferences_item;

namespace detail{


//This object connects different actions from different views to two generic signals:
// item_changed   - hold refference to currently selected item
// button_clicked - hold QDialogButtonBox::StandartButton code
class connector: public QObject{
    Q_OBJECT

public:

    connector(preferences_dialog* dialog, std::list<preferences_item>& items, QDialogButtonBox* buttons);
    ~connector();

public Q_SLOTS:

    void currentItemChanged ( QTreeWidgetItem * current, QTreeWidgetItem * previous );
    void currentItemChanged ( QListWidgetItem * current, QListWidgetItem * previous );
    void currentTabChanged(int);
#ifdef USE_KDE_DIALOG
    void currentPageChanged ( KPageWidgetItem *current, KPageWidgetItem *previous );
#endif

    void buttonClicked( QAbstractButton* button );
#ifdef USE_KDE_DIALOG
    void buttonClicked( KDialog::ButtonCode button );
#endif

Q_SIGNALS:

    ///currently selected item
    void item_changed( const preferences_item& );

    /// \p button equal QDialogButtonBox::StandartButton codes
    void button_clicked( int button );

private:
    QPointer<preferences_dialog> dialog_;
    std::list<preferences_item>& items_;
    QDialogButtonBox* buttons_;
};

} //namespace detail

#endif


