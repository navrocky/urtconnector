
#ifndef CL_CONFIG_PLAIN_F_H
#define CL_CONFIG_PLAIN_F_H

#include <QWidget>

namespace detail { namespace plain {

///Embed item into simple QWidget( Plain confing representation )
struct item_inserter {
    QWidget& form;
    item_inserter( QWidget& form ) :form( form ) {}

    void operator()( preferences_item, preferences_item ) {}
};


///Embed \b item-widget to simple QWidget( Plain confing representation )
struct widget_inserter{
    QWidget& form;
    widget_inserter( QWidget& form ) : form(form) {}

    void operator()( preferences_item item )
    {
        if ( !form.layout() ) (new QVBoxLayout(&form))->setContentsMargins(0,0,0,0);
        form.layout()->addWidget( item.widget() );
    }
};


///Extract widget from simple QWidget( Plain confing representation )
struct widget_extractor{
    QWidget& form;
    widget_extractor( QWidget& form ) :form(form) {}

    preferences_widget* operator()()
    {
       return form.findChild<preferences_widget*>( preferences_widget::object_name );
    }
};


///Set current \b item-widget for simple QWidget( Plain confing representation )
struct widget_set{
    QWidget& form;
    widget_set( QWidget& form ) : form(form) {}

    void operator()( preferences_item ) {}
};


///Set current item  for simple QWidget( Plain confing representation )
struct set_current_item{
    QWidget& form;
    set_current_item( QWidget& form ) : form(form) {}

    void operator()( preferences_item ) {}
};


} //namespace plain


template<>
struct prefferences_form_traits<QWidget>
{
    typedef plain::item_inserter    item_inserter;
    typedef plain::widget_inserter  widget_inserter;
    typedef plain::widget_set       widget_setter;
    typedef plain::widget_extractor widget_extractor;
    typedef plain::set_current_item item_setter;
};


} //namespace detail

#endif
