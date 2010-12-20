
#ifndef CL_CONFIG_LIST_F_H
#define CL_CONFIG_LIST_F_H

#include <QListWidget>

namespace detail { namespace list {


///Insert items to QListWidget
struct item_inserter {
    Ui_list_form& form;
    item_inserter( Ui_list_form& form ) :form( form ) {}
    
    void operator()( preferences_item item, preferences_item parent )
    {
        if (parent)
            throw std::runtime_error( "No child allowed in List-represented config UI" );

        preferences_widget* cw = item.widget();
        QListWidgetItem* it = new QListWidgetItem( cw->icon(), cw->name() );
        it->setData( preferences_item::qobject_property_role, qVariantFromValue( item ) );
        item.set_list_item( it );
        form.items->addItem( it );
    }
};


///Add \b item-widget to List-represented config UI;
struct widget_inserter{
    Ui_list_form& form;
    widget_inserter( Ui_list_form& form ) : form(form) {}

    void operator()( preferences_item item )
    {
        preferences_widget* cw = item.widget();
        form.header->setText( cw->header() );
        form.icon->setIcon( cw->icon() );
        item.set_index( form.pages->addWidget(cw) );
    }
};


///Extract current widget from List-represented config UI;
struct widget_extractor{
    Ui_list_form& form;
    widget_extractor( Ui_list_form& form ) :form(form) {}

    preferences_widget* operator()()
    {
        return static_cast<preferences_widget*>( form.pages->currentWidget() );
    }
};


///Set current \b item-widget into List-represented config UI;
struct widget_set{
    Ui_list_form& form;
    widget_set( Ui_list_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        preferences_widget* cw = item.widget();
        form.pages->setCurrentWidget( cw );
        form.header->setText( cw->header() );
        form.icon->setIcon( cw->icon() );
    }

};


///Set current item into List-represented config UI;
struct set_current_item{
    Ui_list_form& form;
    set_current_item( Ui_list_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        form.items->setCurrentItem( boost::get<QListWidgetItem*>( item.list_item() ) );
    }
};


} //namespace list

template<>
struct prefferences_form_traits<Ui_list_form>
{
    typedef list::item_inserter     item_inserter;
    typedef list::widget_inserter   widget_inserter;
    typedef list::widget_set        widget_setter;
    typedef list::widget_extractor  widget_extractor;
    typedef list::set_current_item  item_setter;
};


} //namespace detail

#endif


