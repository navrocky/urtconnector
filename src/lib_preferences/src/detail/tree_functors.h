

#ifndef CL_CONFIG_TREE_F_H
#define CL_CONFIG_TREE_F_H

#include <QTreeWidget>

namespace detail { namespace tree {

///Insert items to QTreeWidget
struct item_inserter {
    Ui_tree_form& form;
    item_inserter( Ui_tree_form& form ) :form( form ) {}

    void operator()( preferences_item item, preferences_item parent )
    {
        preferences_widget* cw = item.widget();

        QTreeWidgetItem* list_item = new QTreeWidgetItem();
        list_item->setIcon(0, cw->icon() );
        list_item->setText(0, cw->name() );
        list_item->setData(0, preferences_item::qobject_property_role, qVariantFromValue( item ) );

        item.set_list_item( list_item );

        if ( parent )
        {
            QTreeWidgetItem* parent_list_item = boost::get<QTreeWidgetItem*>( parent.list_item() );
            parent_list_item->addChild( list_item );
        }
        else
        {
            form.items->addTopLevelItem(list_item);
        }
    }
};


///Add \b item-widget to Tree-represented config UI;
struct widget_inserter{
    Ui_tree_form& form;
    widget_inserter( Ui_tree_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        preferences_widget* cw = item.widget();
        form.header->setText( cw->header() );
        form.icon->setIcon( cw->icon() );
        item.set_index( form.pages->addWidget(cw) );
    }
};


///Extract current widget from Tree-represented config UI;
struct widget_extractor{
    Ui_tree_form& form;
    widget_extractor( Ui_tree_form& form ) :form(form) {}

    preferences_widget* operator()()
    {
        return static_cast<preferences_widget*>( form.pages->currentWidget() );
    }
};


///Set current \b item-widget into Tree-represented config UI;
struct widget_set{
    Ui_tree_form& form;
    widget_set( Ui_tree_form& form ) : form(form) {}

    void operator()( preferences_item item )
    {
        preferences_widget* cw = item.widget();
        form.pages->setCurrentWidget( cw );
        form.header->setText( cw->header() );
        form.icon->setIcon( cw->icon() );
    }

};


///Set current item into Tree-represented config UI;
struct set_current_item{
    Ui_tree_form& form;
    set_current_item( Ui_tree_form& form ) :form(form) {}

    void operator()( preferences_item item )
    {
        form.items->setCurrentItem( boost::get<QTreeWidgetItem*>( item.list_item() ) );
    }
};

} //namespace tree

template<>
struct prefferences_form_traits<Ui_tree_form>
{
    typedef tree::item_inserter     item_inserter;
    typedef tree::widget_inserter   widget_inserter;
    typedef tree::widget_set        widget_setter;
    typedef tree::widget_extractor  widget_extractor;
    typedef tree::set_current_item  item_setter;
};

} //namespace detail

#endif
