
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <QList>
#include <QToolBar>


#include "common/server_list.h"
#include "filters/filter_list.h"
#include "filters/filter.h"
#include "server_list_widget.h"
#include "history_widget.h"
#include "ui_history_widget.h"

#include "history.h"



const int c_id_role = Qt::UserRole + 1;

//FIXME used by status_item_delegate !!
const int c_suppress_role = Qt::UserRole + 11;

struct history_widget::Pimpl{

    Pimpl(){
        central = new QWidget();
        ui.setupUi(central);

        QHeaderView* hdr = ui.treeWidget->header();

        hdr->resizeSection(0, 300);
        hdr->resizeSection(1, 80);
        hdr->resizeSection(2, 200);
    }

    void retranslate(){ ui.retranslateUi(central); }
    
    Ui::history_widget ui;
    QWidget* central;
    
    history_p history;

    server_list_p serv_list;
};

history_widget::history_widget( QWidget *parent, history_p list)
    : main_tab("history_tab", parent)
    , p_( new Pimpl )
{
    p_->history = list;

    init_filter_toolbar();

    setCentralWidget( p_->central );
    

    



    
//     connect(p_->ui.clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));
}

int history_widget::num_rows() const
{
    return p_->ui.treeWidget->topLevelItemCount();
}

history_widget::~history_widget()
{}

void history_widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        p_->retranslate();
        break;
    default:
        break;
    }
}

void history_widget::filter_clear()
{
//     p_->ui.filterEdit->clear();
}

QTreeWidget* history_widget::tree() const
{
    return p_->ui.treeWidget;
}

void history_widget::update_history()
{
   p_->ui.treeWidget->clear();
   int i;
   for (i = 0; i < p_->history->length(); i++)
   {
       addItem(p_->history->at(i));
   }
}

void history_widget::addItem(history_item_p item)
{
    QTreeWidgetItem* item_ptr = new QTreeWidgetItem();
    
    item_ptr->setText(0, item->server_name());
    item_ptr->setText(2, item->date_time().toString( Qt::DefaultLocaleShortDate ));
    item_ptr->setText(3, item->address());
    item_ptr->setText(4, item->password());
    item_ptr->setText(5, item->player_name());
    item_ptr->setData( 0, c_id_role, QVariant::fromValue( item->id() ) );
    item_ptr->setData( 1, c_id_role, QVariant::fromValue( item->id() ) );

    if( QTreeWidgetItem* parent = add_tem( item_ptr ) )
        resort(parent);
}

QTreeWidgetItem* history_widget::add_tem(QTreeWidgetItem* item)
{
    QTreeWidgetItem* parent = find_item( item->data( 0, c_id_role ).value<server_id>() );

    if( parent )
    {
        item->setData( 1, c_suppress_role, true );
        parent->insertChild( 0, item );
        return parent;
    }
    else
    {
        item->setData( 1, c_suppress_role, false );
        p_->ui.treeWidget->insertTopLevelItem(0, item);
        return 0;
    }
}


server_id history_widget::selected_server() const
{
    QTreeWidgetItem* item = p_->ui.treeWidget->currentItem();
    if( !item )
        return server_id();

    return item->data(0, c_id_role).value<server_id>();
}

void history_widget::servers_updated()
{
    p_->ui.treeWidget->setItemDelegateForColumn( 1, new status_item_delegate( server_list(), p_->ui.treeWidget) );
}

void history_widget::filter_changed()
{
    QList<QTreeWidgetItem *> items = p_->ui.treeWidget->findItems ( "", Qt::MatchStartsWith );

    foreach( QTreeWidgetItem * item, items) {
        
        server_info_p si = server_list()->get( item->data(0, c_id_role).value<server_id>() );
        item->setHidden( !filterlist().filtrate( *si ) );
    }
}



QTreeWidgetItem* history_widget::find_item(const server_id& id) const
{
    for (int i = 0; i < p_->ui.treeWidget->topLevelItemCount(); ++i){
        QTreeWidgetItem* item = p_->ui.treeWidget->topLevelItem(i);
        if( item->data(0, c_id_role).value<server_id>() == id )
            return item;
    }

    return 0;
}

void history_widget::resort( QTreeWidgetItem* item )
{
    int index = p_->ui.treeWidget->indexOfTopLevelItem ( item );
    
    if (index == -1) return;
    
    item = p_->ui.treeWidget->takeTopLevelItem ( index );

    QList<QTreeWidgetItem*> chlds = item->takeChildren();

    //item itself is under resorting too!
    chlds << item;
    
    std::sort( chlds.begin(), chlds.end(),
        boost::bind( &QTreeWidgetItem::text, _1, 0 ) < boost::bind( &QTreeWidgetItem::text, _2, 0 ) );

    std::for_each( chlds.begin(), chlds.end(), boost::bind( &history_widget::add_tem, this, _1) );
}
