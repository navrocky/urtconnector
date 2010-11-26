
#ifndef CL_CONFIG_TAB_F_H
#define CL_CONFIG_TAB_F_H

#include <QTabWidget>

namespace detail { namespace tab {

///Insert items to QTabWidget
struct item_inserter {
    Ui_tab_form& form;
    item_inserter( Ui_tab_form& form ) :form( form ) {}

    void operator()( preferences_item item, preferences_item parent )
    {
        preferences_widget* cw = item.widget();
        if ( parent )
        {
            if ( parent.parent() )
                throw std::runtime_error( "no child allowed in tabs" );
            item.set_list_item( parent.list_item() );
        }
        else
        {
            QListWidgetItem* list_item = new QListWidgetItem( cw->icon(), cw->name() );
            list_item->setData( preferences_item::qobject_property_role, qVariantFromValue( item ) );
            item.set_list_item( list_item );
            form.items->addItem( list_item );
        }
    }
};

void make_tab( QTabWidget* tw, preferences_item item )
{
    QWidget* tab = new QWidget(0);
    QVBoxLayout* vlay = new QVBoxLayout(tab);
    vlay->setContentsMargins(2,2,2,2);
    vlay->setSpacing(5);

    preferences_widget* cw = item.widget();

    if ( !cw->header().isEmpty() )
    {
        QLabel* header = new QLabel( cw->header() );
        vlay->addWidget( header );
    }

    vlay->addWidget( cw );
    cw->show();
    vlay->addStretch();

    int index = tw->addTab( tab, cw->icon(), cw->name() );
    item.set_index( index );
}

///Add \b item-widget to Tab-represented config UI;
struct widget_inserter {
    Ui_tab_form& form;
    widget_inserter( Ui_tab_form& form ) :form(form) {}

    void operator()(  preferences_item item )
    {
        if ( form.items->currentItem() == boost::get<QListWidgetItem*>( item.list_item() ) )
            make_tab(form.pages, item);
    }
};


///Extract current widget from Tab-represented config UI;
struct widget_extractor{
    Ui_tab_form& form;
    widget_extractor( Ui_tab_form& form ) :form(form) {}

    preferences_widget* operator()()
    {
        QWidget* w = form.pages->currentWidget();
        if( !w ) return 0;
        return w->findChild<preferences_widget*>( preferences_widget::object_name );
    }
};

///Set current \b item-widget into Tab-represented config UI;
struct widget_set{
    Ui_tab_form& form;

    widget_set( Ui_tab_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        if ( item.index() != -1 )
        {
            form.pages->setCurrentIndex( item.index() );
        }
        else
        {
            clear();
            make_tab(form.pages, item);
            std::list<preferences_item> childs = item.childs();
            for_each( childs.begin(), childs.end(),
                boost::bind(detail::tab::make_tab, boost::ref(form.pages), _1) );
        }
    }

    void clear()
    {
        for(int i = 0; i<form.items->count(); ++i )
        {
            QVariant data = form.items->item(i)->data( preferences_item::qobject_property_role );
            preferences_item item = data.value<preferences_item>();
            item.summon();
        }

        while ( QWidget* w = form.pages->currentWidget() )
        {
            delete w;
        }

        form.pages->clear();
    }
};


///Set current item into Tree-represented config UI;
struct set_current_item{
    Ui_tab_form& form;
    set_current_item( Ui_tab_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        form.items->setCurrentItem( boost::get<QListWidgetItem*>( item.list_item() ) );
    }
};

} //namespace tab

template<>
struct prefferences_form_traits<Ui_tab_form>
{
    typedef tab::item_inserter      item_inserter;
    typedef tab::widget_inserter    widget_inserter;
    typedef tab::widget_set         widget_setter;
    typedef tab::widget_extractor   widget_extractor;
    typedef tab::set_current_item   item_setter;
};


} //namespace detail

#endif
