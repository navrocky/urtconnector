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

#include <iostream>
#include <stdexcept>

#include <boost/bind.hpp>

#include <QPointer>
#include <QPushButton>
#include <QMessageBox>

#include "ui_preferences_dialog.h"
#include "ui_tree_form.h"
#include "ui_tab_form.h"
#include "ui_list_form.h"

#include "detail/functors.h"
#include "detail/connector.h"

#include "preferences_widget.h"
#include "preferences_dialog.h"

#ifndef USE_KDE_DIALOG
    const int kde_native = 0;
#else
    #include <kpagedialog.h>
    #include <kpushbutton.h>
    const int kde_native = 1;
#endif

struct plain_tag;
struct tree_tag;
struct tab_tag;
struct list_tag;
struct kde_native_tag;

template <typename Tag>
struct preferences_dialog_traits{};


template <>
struct preferences_dialog_traits<plain_tag>
{
    typedef QWidget form;
    static const preferences_dialog::Type type = preferences_dialog::Plain;
};

template <>
struct preferences_dialog_traits<tree_tag>
{
    typedef Ui_tree_form form;
    static const preferences_dialog::Type type = preferences_dialog::Tree;
};

template <>
struct preferences_dialog_traits<tab_tag>
{
    typedef Ui_tab_form form;
    static const preferences_dialog::Type type = preferences_dialog::Tab;
};

template <>
struct preferences_dialog_traits<list_tag>
{
    typedef Ui_list_form form;
    static const preferences_dialog::Type type = preferences_dialog::List;
};

#ifdef USE_KDE_DIALOG

template <>
struct preferences_dialog_traits<kde_native_tag>
{
    typedef KPageDialog form;
    static const int type = -1;
};

KPageDialog::FaceType kpage_type( preferences_dialog::Type type )
{
    switch( type )
    {
        case preferences_dialog::Tree:  return KPageDialog::Tree;
        case preferences_dialog::Tab:   return KPageDialog::Tabbed;
        case preferences_dialog::List:  return KPageDialog::List;
        case preferences_dialog::Plain: return KPageDialog::Plain;
        default: return KPageDialog::Auto;
    }
}

KDialog::ButtonCode kbutton( QDialogButtonBox::StandardButton button )
{
    switch (button)
    {
        case QDialogButtonBox::Apply:   return KDialog::Apply;
        case QDialogButtonBox::Cancel:  return KDialog::Cancel;
        case QDialogButtonBox::Ok:      return KDialog::Ok;
        case QDialogButtonBox::RestoreDefaults:      return KDialog::Default;
        default: return KDialog::None;
    }
}

#endif


/// struct that creates and holds concrete helpers as generic functors
struct preferences_dialog::Pimpl { 

    typedef boost::variant<
        preferences_dialog_traits<tree_tag>::form,
        preferences_dialog_traits<tab_tag>::form,
        preferences_dialog_traits<list_tag>::form>  PreferencesForm;

    Pimpl():template_widget(0){}

    template <typename Tag>
    typename preferences_dialog_traits<Tag>::form& init_ui( QWidget* template_widget  )
    {
        typedef typename preferences_dialog_traits<Tag>::form Form;

        Form& form_ui = boost::get<Form>( preferences_ui = Form() );

        form_ui.setupUi(template_widget);
        init_functors(form_ui);
        type = preferences_dialog_traits<Tag>::type;
        return form_ui;
    }
#ifdef USE_KDE_DIALOG
    template <typename Tag>
    typename preferences_dialog_traits<Tag>::form& init_ui( KPageDialog* page )
    {
        init_functors(*page);
        return *page;
    }
#endif

    template <typename Form>
    void init_functors( Form& form_ui )
    {
        add_item    = make_item_inserter( form_ui );
        add_widget  = make_widget_inserter( form_ui );
        set_current_widget = make_widget_setter(form_ui);
        current_widget  = make_widget_extractor( form_ui );
        set_current_item = make_item_setter( form_ui );
    }

    //main dialog ui object
    Ui_preferences_dialog_form ui;
    //current Type-specific ui object
    PreferencesForm     preferences_ui;

    //generic add_widget-function
    WidgetInserter   add_widget;
    //generic add_item-function
    ItemInserter     add_item;
    //generic current_widget-function
    CurrentWidgetExtractor  current_widget;
    //generic set_current_widget-function
    CurrentWidgetSetter set_current_widget;
    //generic set_current_item-function
    CurrentItemSetter set_current_item;

    std::list<preferences_item> items;

    preferences_item    current_item;

    Type initial_type;
    Type type;

    std::auto_ptr<QWidget> template_widget;

#ifdef USE_KDE_DIALOG
    std::auto_ptr<KPageDialog> page_dialog;
#endif

    detail::connector* connector;
    bool native;
};




preferences_dialog::preferences_dialog(Type t, bool native, QWidget * parent, Qt::WindowFlags f)
    : QDialog( parent, f )
    , p_( new Pimpl )
{
    p_->initial_type = t;
    p_->native = native && kde_native;

    if ( kde_native && p_->native  )
        setup_native_ui();
    else
        setup_ui();

    button( QDialogButtonBox::RestoreDefaults )->setAutoDefault(false);
    button( QDialogButtonBox::Ok )->setDefault(true);

    connect(p_->connector,SIGNAL( item_changed( preferences_item ) ),
            SLOT( item_changed( preferences_item ) ) );

    connect(p_->connector,SIGNAL( button_clicked( int ) ),
            SLOT( button_clicked( int ) ) );

    reset();
}

preferences_dialog::~ preferences_dialog()
{}

void preferences_dialog::setup_ui()
{
    p_->ui.setupUi(this);
    p_->connector = new detail::connector( this, p_->items, p_->ui.buttons);

    switch (p_->initial_type)
    {
        case Tree: setup_tree(); break;
        case Tab:  setup_tabs(); break;
        case List: setup_list(); break;
        case Auto: setup_plain(); break;
        case Plain: setup_plain(); break;
    }

    connect(p_->ui.buttons, SIGNAL( clicked( QAbstractButton* ) ),
        p_->connector, SLOT( buttonClicked( QAbstractButton* ) ) );
}

void preferences_dialog::setup_native_ui()
{
#ifdef USE_KDE_DIALOG
    p_->connector = new detail::connector( this, p_->items, 0);

    ( new QVBoxLayout(this) )->setContentsMargins(0,0,0,0);

    setup_clear();

    connect(p_->page_dialog.get(), SIGNAL(currentPageChanged ( KPageWidgetItem *, KPageWidgetItem * )),
        p_->connector, SLOT(currentPageChanged ( KPageWidgetItem *, KPageWidgetItem * )));

    connect(p_->page_dialog.get(), SIGNAL( buttonClicked( KDialog::ButtonCode ) ),
        p_->connector, SLOT( buttonClicked( KDialog::ButtonCode ) ) );
#endif
}

void preferences_dialog::setup_clear()
{
    for_each( p_->items.begin(), p_->items.end(), bind(&preferences_item::summon, _1) );

#ifdef USE_KDE_DIALOG
    if ( kde_native && p_->native )
    {
        p_->page_dialog.reset( new KPageDialog );
        layout()->addWidget( p_->page_dialog.get() );
        p_->page_dialog->setFaceType( kpage_type( p_->initial_type ) );
        p_->page_dialog->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply | KDialog::Default );
        p_->page_dialog->setDefaultButton (KDialog::Ok );
        p_->init_ui<kde_native_tag>( p_->page_dialog.get() );
    }
    else
#endif
    {
        p_->template_widget.reset( new QWidget );
        p_->ui.main_widget->layout()->addWidget( p_->template_widget.get() );
    }
}


void preferences_dialog::setup_plain()
{
    setup_clear();
    p_->init_functors( *p_->template_widget.get() );
}

void preferences_dialog::setup_tree()
{
    setup_clear();
    preferences_dialog_traits<tree_tag>::form& form_ui = p_->init_ui<tree_tag>( p_->template_widget.get() );

    connect(form_ui.items, SIGNAL(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)),
        p_->connector, SLOT(currentItemChanged(QTreeWidgetItem *, QTreeWidgetItem *)));
}

void preferences_dialog::setup_tabs()
{
    setup_clear();
    preferences_dialog_traits<tab_tag>::form& form_ui = p_->init_ui<tab_tag>( p_->template_widget.get() );

    connect(form_ui.items,  SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
        p_->connector, SLOT(currentItemChanged(QListWidgetItem *, QListWidgetItem *)));
    connect(form_ui.pages,  SIGNAL(currentChanged ( int )),
        p_->connector, SLOT(currentTabChanged ( int )) );
}

void preferences_dialog::setup_list()
{
    setup_clear();
    preferences_dialog_traits<list_tag>::form& form_ui = p_->init_ui<list_tag>( p_->template_widget.get() );

    connect(form_ui.items, SIGNAL(currentItemChanged(QListWidgetItem *, QListWidgetItem *)),
        p_->connector, SLOT(currentItemChanged(QListWidgetItem *, QListWidgetItem *)));
}

preferences_item preferences_dialog::add_item(preferences_widget * cw, const preferences_item& parent )
{
    if ( p_->initial_type == Plain && p_->items.size() )
        throw std::runtime_error("plain type occured!");
    std::cerr<<"ai1"<<std::endl;
    preferences_item item( cw, parent );
    p_->items.push_back(item);

    if ( p_->items.size() == 1)
        p_->current_item = item;
    
    std::cerr<<"ai2"<<std::endl;
    connect( cw, SIGNAL( changed() ), SLOT( changed() ) );
    if ( !p_->native )
    {
        if ( p_->initial_type == Auto )
        {
            Type t = calculate_type();
            if ( t != p_->type )
            {
                p_->type = t;
                switch (p_->type)
                {
                    case Tree: setup_tree(); break;
                    case Tab:  setup_tabs(); break;
                    case List: setup_list(); break;
                    default:;
                }
                std::list<preferences_item>::iterator it = p_->items.begin();
                for(; it != p_->items.end(); ++it)
                {
                    p_->add_item( *it, it->parent() );
                    p_->add_widget( *it );
                }
                if( p_->current_item )
                {
                    p_->set_current_item( p_->current_item );
                    p_->current_item.widget()->update_preferences();
                    p_->set_current_widget( p_->current_item );
                }
                return item;
            }
        }
    }
    std::cerr<<"ai3"<<std::endl;
    p_->add_item( item, parent );
    std::cerr<<"ai4"<<std::endl;
    p_->add_widget( item );
    std::cerr<<"ai5"<<std::endl;

    if ( p_->items.size() == 1 )
    {
        std::cerr<<"ai6"<<std::endl;
        p_->current_item = item;
        std::cerr<<"ai7"<<std::endl;
        p_->set_current_item( p_->current_item );
        std::cerr<<"ai8"<<std::endl;
        p_->current_item.widget()->update_preferences();
        p_->set_current_widget( p_->current_item );
        std::cerr<<"ai9"<<std::endl;
    }

    return item;
}

void preferences_dialog::changed()
{
    button( QDialogButtonBox::RestoreDefaults )->setEnabled(true);
    button( QDialogButtonBox::Apply )->setEnabled(true);
}

void preferences_dialog::accept()
{ okClicked(); }

void preferences_dialog::reject()
{ cancelClicked(); }

void preferences_dialog::apply_current()
{
    p_->current_item.widget()->accept();
    button( QDialogButtonBox::Apply )->setEnabled(false);
}

void preferences_dialog::reject_current()
{
    p_->current_item.widget()->reject();
    button( QDialogButtonBox::Apply )->setEnabled(false);
}

void preferences_dialog::restoreClicked()
{
    QMessageBox::StandardButton ret = QMessageBox::question ( this,
        tr("Восстановить настройки по умолчанию"),
        tr("Действительно восстановить настройки по умолчанию?" ),
        QMessageBox::Ok |  QMessageBox::Cancel );

    if ( ret == QMessageBox::Cancel ) return;

    if ( button(QDialogButtonBox::Apply)->isEnabled() ) reject_current();
    p_->current_item.widget()->reset_defaults();
    button( QDialogButtonBox::RestoreDefaults )->setEnabled(false);
    button( QDialogButtonBox::Apply )->setEnabled(false);
}

void preferences_dialog::applyClicked()
{ apply_current(); }


void preferences_dialog::okClicked()
{
    if ( button(QDialogButtonBox::Apply)->isEnabled() ) apply_current();
    QDialog::accept();
}

void preferences_dialog::cancelClicked()
{
    if ( button(QDialogButtonBox::Apply)->isEnabled() ) reject_current();
    QDialog::reject();
}

const preferences_item& preferences_dialog::current_item() const
{
    return p_->current_item;
}

const preferences_widget* preferences_dialog::current_widget() const
{
    return p_->current_widget();
}

void preferences_dialog::button_clicked(int button)
{
    switch ( button )
    {
        case QDialogButtonBox::RestoreDefaults: restoreClicked(); break;
        case QDialogButtonBox::Ok:              okClicked(); break;
        case QDialogButtonBox::Apply:           applyClicked(); break;
        case QDialogButtonBox::Cancel:          cancelClicked(); break;
        default:
            std::cerr<<"unknown button"<<std::endl;
    }
}

void preferences_dialog::item_changed(const preferences_item& new_item)
{
    std::cerr<<"ic1:"<<new_item<<std::endl;
    if( new_item == p_->current_item ) return;
    std::cerr<<"ic2"<<std::endl;
    if( button(QDialogButtonBox::Apply)->isEnabled() )
    {
        QMessageBox::StandardButton ret = QMessageBox::question ( this,
            tr("Применить настройки"),
            tr("В текущем модуле имеются несохранённые изменения.\n Применить их?" ),
            QMessageBox::Ok | QMessageBox::Reset | QMessageBox::Cancel );

        if ( ret == QMessageBox::Ok )
        {
            apply_current();
        }
        else if( ret == QMessageBox::Reset )
        {
            reject_current();
        }
        else if( ret == QMessageBox::Cancel )
        {
            p_->current_item.widget()->update_preferences();
            p_->set_current_widget( p_->current_item );
            return;
        }
    }
    std::cerr<<"ic3"<<std::endl;
    p_->current_item = new_item;
    std::cerr<<"ic4"<<std::endl;
    p_->current_item.widget()->update_preferences();
    p_->set_current_widget( p_->current_item );
    std::cerr<<"ic5"<<std::endl;
    p_->set_current_item( p_->current_item );
    std::cerr<<"ic6"<<std::endl;
    return;
}

preferences_dialog::Type preferences_dialog::calculate_type() const
{
    if ( p_->items.size() == 1 )
        return Plain;
    std::list<preferences_item>::iterator it = p_->items.begin();
    Type ret = List;
    for (; it != p_->items.end(); ++it)
    {
        if ( it->parent() )
        {
            ret = Tab;
            if ( it->parent().parent() )
            {
                ret = Tree;
                break;
            }
        }
    }
    return ret;
}


void preferences_dialog::reset()
{
    button( QDialogButtonBox::RestoreDefaults )->setEnabled(true);
    button( QDialogButtonBox::Ok )->setEnabled(true);
    button( QDialogButtonBox::Cancel )->setEnabled(true);
    button( QDialogButtonBox::Apply )->setEnabled(false);
}

QPushButton * preferences_dialog::button(int button) const
{
    QPushButton* ret(0);

#ifdef USE_KDE_DIALOG
    if ( kde_native && p_->native )
    {
        ret =  p_->page_dialog->button( kbutton( QDialogButtonBox::StandardButton(button) ) );
    }
    else
#endif
    ret =  p_->ui.buttons->button( QDialogButtonBox::StandardButton(button) );

    if ( !ret )
        throw std::runtime_error("no such button");
    return ret;
}









