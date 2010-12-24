
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <QMainWindow>
#include <QDockWidget>
#include <QToolBar>
#include <QAction>
#include <QBoxLayout>
#include <QPointer>
#include <QSettings>

#include "common/server_list.h"
#include "common/qaccumulatingconnection.h"
#include "filters/filter_list.h"
#include "filters/filter_factory.h"
#include "filters/regexp_filter.h"
#include "filters/composite_filter.h"
#include "filters/filter_edit_widget.h"
#include "filters/tools.h"
#include "common/state_settings.h"

#include "tools.h"
#include "main_tab.h"

namespace {
void correct_names(filter_list_p fl, filter_p par)
{
    if (par->name().isEmpty())
        par->set_name(fl->correct_name(par->get_class()->caption()));

    composite_filter* cf = dynamic_cast<composite_filter*>(par.get());
    if (!cf)
        return;
    foreach (filter_p f, cf->filters())
        correct_names(fl, f);
}
}

struct main_tab::Pimpl{

    Pimpl( const QString& object_name, filter_factory_p factory )
        : updater( 500, QAccumulatingConnection::Periodically )
        , filter_widget(0)
        , filter_holder(0)
        , filters( new filter_list(factory) )
        , st(object_name)
    {
        if( !factory->has_filter( regexp_filter_class::get_id() ) )
            factory->add_class( filter_class_p( new regexp_filter_class ) );

        if( !factory->has_filter( composite_filter_class::get_id() ) )
            factory->add_class( filter_class_p( new composite_filter_class ) );
    }

    QAccumulatingConnection updater;
    server_list_p           servers;
    QPointer<QDockWidget>   filter_widget;
    QWidget*                filter_holder;
    filter_list_p           filters;

    tab_settings            st;
};



main_tab::main_tab( const QString& object_name, QWidget* parent, filter_factory_p factory )
    : QMainWindow(parent)
    , p_( new Pimpl( object_name, (factory) ? factory : filter_factory_p( new filter_factory ) ) )
{
    setObjectName(object_name);
    
    setWindowFlags( windowFlags() & (~Qt::Window) );
    
    connect( &p_->updater, SIGNAL( signal() ), this, SLOT( servers_updated() ) );

    try
    {
        filter_p f = p_->st.root_filter( p_->filters->factory() );
        p_->filters->set_root_filter(f);
        correct_names(p_->filters, p_->filters->root_filter());

        QString name = p_->st.toolbar_filter();
        f = p_->filters->find_by_name(name);
        p_->filters->set_toolbar_filter(f);
    }
    catch( const std::exception& )
    {
        filter_p f = p_->filters->create_by_class_id( composite_filter_class::get_id() );
        f->set_name( f->get_class()->caption() );
        p_->filters->set_root_filter(f);
        composite_filter* cf = dynamic_cast<composite_filter*>(f.get());

//         create regexp filter as composite child
        f = p_->filters->create_by_class_id(regexp_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        cf->add_filter(f);

//         select regexp filter for toolbar
        p_->filters->set_toolbar_filter(f);
    }

    connect( p_->filters->root_filter().get(), SIGNAL( changed_signal() ),
        this, SLOT  ( filter_changed() ) );
    
    connect( p_->filters.get(), SIGNAL( toolbar_filter_changed() ),
        this, SLOT  ( update_toolbar_filter()  ) );


    
    connect( p_->filters->root_filter().get(), SIGNAL( changed_signal() ),
        this, SLOT  ( save_filter() ) );

    connect( p_->filters.get(), SIGNAL( toolbar_filter_changed() ),
        this, SLOT  ( save_filter() ) );

}

main_tab::~main_tab()
{}

void main_tab::init_filter_toolbar()
{
    QToolBar* tb = new QToolBar( QObject::tr("Filter toolbar"), this);
    tb->setObjectName("filter_toolbar");
    addToolBar( Qt::TopToolBarArea, tb );

    QAction* show_filter_a = new QAction(QIcon(":/icons/icons/view-filter.png"), QObject::tr("View and edit filter"), this);
    show_filter_a->setCheckable(true);

    show_filter_a->setChecked( p_->st.is_filter_visible() );
    
    connect( show_filter_a, SIGNAL( toggled(bool) ), this,  SLOT( show_filter(bool) ) );
    
    tb->addAction(show_filter_a);

    p_->filter_holder = new QWidget();
    QBoxLayout* lay = new QBoxLayout( QBoxLayout::LeftToRight, p_->filter_holder );
    lay->setContentsMargins(0, 0, 0, 0);
    tb->addWidget(p_->filter_holder);

    update_toolbar_filter();

    p_->filter_widget = new QDockWidget( QObject::tr("Filter"), this );
    p_->filter_widget->setObjectName("filter_widget");
    filter_edit_widget* filter = new filter_edit_widget( p_->filters, p_->filter_widget );
    p_->filter_widget->setWidget( filter );
    p_->filter_widget->setVisible( p_->st.is_filter_visible() );
    addDockWidget( Qt::LeftDockWidgetArea, p_->filter_widget );

    connect( p_->filter_widget, SIGNAL( dockLocationChanged( Qt::DockWidgetArea ) ), this, SLOT( save_filter() ) );

    restoreState( p_->st.load_state(), 1);
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

void main_tab::show_filter(bool b)
{
    p_->filter_widget->setVisible(b);
}

void main_tab::save_filter()
{
    p_->st.save_root_filter( p_->filters->root_filter() );

    filter_p tbf = p_->filters->toolbar_filter().lock();

    if (tbf)
        p_->st.save_toolbar_filter( tbf->name() );
    else
        p_->st.save_toolbar_filter( "" );

    p_->st.save_state( saveState(1) );
    p_->st.set_filter_visible( p_->filter_widget->isVisible() );
}


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
{ return p_->servers; }

void main_tab::force_servers_update()
{ p_->updater.emitSignal(); }

const filter_list& main_tab::filterlist() const
{ return *p_->filters; }




////////////////////////////////////////////////////////////////////////////////
// server_tree

tab_settings::tab_settings(const QString& object_name)
{
    base_settings set;
    set.register_sub_group( object_name, object_name, state_settings::uid() );
    st = base_settings().get_settings( object_name );
}

filter_p tab_settings::root_filter(filter_factory_p factory) const
{
    QByteArray ba = st->value("root_filter").toByteArray();
    return filter_load(ba, factory);
}

void tab_settings::save_root_filter(filter_p f)
{
    st->setValue("root_filter", filter_save(f));
}

QString tab_settings::toolbar_filter() const
{
    return st->value("toolbar_filter_name").toString();
}

void tab_settings::save_toolbar_filter(const QString& name)
{
    st->setValue("toolbar_filter_name", name);
}

void tab_settings::save_state(const QByteArray& a)
{
    st->setValue("state", a);
}

QByteArray tab_settings::load_state() const
{
    return st->value("state").toByteArray();;
}

bool tab_settings::is_filter_visible() const
{
    return st->value("filter_visible").toBool();
}

void tab_settings::set_filter_visible(bool val)
{
    st->setValue("filter_visible", val);
}




