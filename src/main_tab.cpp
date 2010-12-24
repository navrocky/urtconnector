
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <QMainWindow>
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QBoxLayout>
#include <QPointer>

#include "common/server_list.h"
#include "common/qaccumulatingconnection.h"
#include "filters/filter_list.h"
#include "filters/filter_factory.h"
#include "filters/regexp_filter.h"
#include "filters/composite_filter.h"
#include "filters/filter_edit_widget.h"

#include "tools.h"
#include "main_tab.h"


struct main_tab::Pimpl{

    Pimpl( QMainWindow* mw, filter_factory_p factory )
        : self(mw)
        , updater( 500, QAccumulatingConnection::Periodically )
        , filter_widget(0)
        , filter_holder(0)
        , filters( new filter_list(factory) )
    {
        if( !factory->has_filter( regexp_filter_class::get_id() ) )
            factory->add_class( filter_class_p( new regexp_filter_class ) );

        if( !factory->has_filter( composite_filter_class::get_id() ) )
            factory->add_class( filter_class_p( new composite_filter_class ) );
    }

    QMainWindow*    self;
    server_list_p   servers;    
    QAccumulatingConnection updater;

    QPointer<QDockWidget>   filter_widget;
    QWidget*        filter_holder;
    filter_list_p   filters;
};

void main_tab::init_main_tab( QMainWindow* mw, filter_factory_p factory )
{
    p_.reset( new Pimpl(mw, ( factory ) ? factory : filter_factory_p( new filter_factory ) ) );

    p_->self->setWindowFlags( p_->self->windowFlags() & (~Qt::Window) );
    QObject::connect( &p_->updater, SIGNAL( signal() ), p_->self, SLOT( servers_updated() ) );

    filter_p f = p_->filters->create_by_class_id( composite_filter_class::get_id() );
    f->set_name( f->get_class()->caption() );
    p_->filters->set_root_filter(f);
    composite_filter* cf = dynamic_cast<composite_filter*>(f.get());

    // create regexp filter as composite child
    f = p_->filters->create_by_class_id(regexp_filter_class::get_id());
    f->set_name(f->get_class()->caption());
    cf->add_filter(f);

    // select regexp filter for toolbar
    p_->filters->set_toolbar_filter(f);

    QObject::connect(
        p_->filters->root_filter().get(), SIGNAL( changed_signal() ),
        p_->self,                        SLOT  ( filter_changed() )
    );
    QObject::connect(
        p_->filters.get(), SIGNAL( toolbar_filter_changed() ),
        new qt_signal_wrapper( p_->filters.get(), boost::bind( &main_tab::update_toolbar_filter, this ) ), SLOT  ( activate()  )
    );
}

void main_tab::init_filter_toolbar()
{
    QToolBar* tb = new QToolBar( QObject::tr("Filter toolbar"), p_->self);
    p_->self->addToolBar( Qt::TopToolBarArea, tb );

    QAction* show_filter = new QAction(QIcon(":/icons/icons/view-filter.png"), QObject::tr("View and edit filter"), p_->self);
    show_filter->setCheckable(true);
    
    QObject::connect(
        show_filter, SIGNAL(triggered()),
        new qt_signal_wrapper( show_filter, boost::bind( &main_tab::edit_filter, this ) ), SLOT( activate() )
    );
    
    tb->addAction(show_filter);

    p_->filter_holder = new QWidget( p_->self );
    QBoxLayout* lay = new QBoxLayout( QBoxLayout::LeftToRight, p_->filter_holder );
    lay->setContentsMargins(0, 0, 0, 0);
    tb->addWidget(p_->filter_holder);

    update_toolbar_filter();
}

void main_tab::update_toolbar_filter()
{
    if( !p_->filter_holder ) return;

    // remove old
    BOOST_FOREACH ( QObject* o, p_->filter_holder->children() )
        if ( qobject_cast<QWidget*>(o) ) delete o;

    // create new
    filter_p f = p_->filters->toolbar_filter().lock();
    if (f)
    {
        QWidget* w = f->get_class()->create_quick_opts_widget(f);
        if (w)
            p_->filter_holder->layout()->addWidget(w);

        p_->filter_holder->setToolTip(f->get_class()->caption());
    }
}

void main_tab::edit_filter()
{
    if ( p_->filter_widget.isNull() )
    {
        p_->filter_widget = new QDockWidget( QObject::tr("Filter"), p_->self );
        filter_edit_widget* filter = new filter_edit_widget( p_->filters, p_->filter_widget );
        p_->filter_widget->setWidget( filter );
        p_->self->addDockWidget( Qt::LeftDockWidgetArea, p_->filter_widget );
    }
    else
        delete p_->filter_widget;
}


main_tab::~main_tab()
{}

server_id main_tab::selected_server() const
{ return server_id(); };

void main_tab::set_server_list(server_list_p ptr)
{
    if (p_->servers)
        QObject::disconnect( p_->servers.get(), SIGNAL( changed() ), &p_->updater, SLOT( emitSignal() ));

    p_->servers = ptr;
    if (p_->servers)
        QObject::connect( p_->servers.get(), SIGNAL( changed() ), &p_->updater, SLOT( emitSignal() ));

    p_->updater.emitSignal();
}

server_list_p main_tab::server_list() const
{
    return p_->servers;
}

void main_tab::force_servers_update()
{ p_->updater.emitSignal(); }

const filter_list& main_tab::filterlist() const
{ return *p_->filters; }




