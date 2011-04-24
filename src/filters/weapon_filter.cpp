
#include <boost/foreach.hpp>

#include <QHBoxLayout>
#include <QCheckBox>
#include <QResizeEvent>
#include <QButtonGroup>

#include <cl/except/error.h>

#include "weapon_filter.h"

Q_DECLARE_METATYPE(Gear);

weapon_filter_quick_opt_widget::weapon_filter_quick_opt_widget(filter_p f, QWidget* parent)
    : QGroupBox( parent )
    , filter_( *static_cast<weapon_filter*>( f.get() ) )

{
    setTitle("Weapon types allowed");
    
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    
    group_ = new QButtonGroup(this);
    group_->setExclusive(false);
    
    BOOST_FOREACH(Gear g, forbidden( AllGear ) ) {
        QCheckBox* cb = new QCheckBox( gear(g), this );
        cb->setProperty("gear", g);
        
        group_->addButton(cb, g);

        lay->addWidget(cb);
    }
    
    connect( group_, SIGNAL( buttonClicked(int) ), SLOT( clicked(int) ) );
    connect( &filter_, SIGNAL( changed_signal() ), SLOT( filter_changed() ) );

    lay->addStretch();
    
    filter_changed();
}


void weapon_filter_quick_opt_widget::clicked( int gear )
{
    ( group_->button(gear)->isChecked() )
        ? filter_.insert( Gear(gear) )
        : filter_.erase( Gear(gear) );    
}

void weapon_filter_quick_opt_widget::filter_changed()
{
    BOOST_FOREACH(Gear g, forbidden( AllGear ) ) {
        group_->button( g )->setChecked( filter_.available().find(g) != filter_.available().end() );
    }
}


////////////////////////////////////////////////////////////////////////////////
// weapon_filter_class

weapon_filter_class::weapon_filter_class()
: filter_class(get_id(), QObject::tr("Weapon filter"),
               QObject::tr("Hides all servers thats don's match selected weapons."))
{}


QWidget* weapon_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new weapon_filter_quick_opt_widget( f, parent );
}


filter_p weapon_filter_class::create_filter()
{
    return filter_p( new weapon_filter( shared_from_this() ) );
}

const char* weapon_filter_class::get_id()
{
    return "weapon_filter";
}


////////////////////////////////////////////////////////////////////////////////
// weapon_filter

weapon_filter::weapon_filter(filter_class_p fc)
    : filter(fc)
    , available_( forbidden( AllGear ) )
{}

std::set<Gear> interserction( const std::set<Gear>& s1, const std::set<Gear>& s2 )
{
    std::set<Gear> ret;
    std::set_intersection( s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter( ret, ret.end() ) );
    return ret;
}

bool weapon_filter::filter_server( const server_info& si )
{
    if (!enabled())
        return true;

    return interserction( si.forbidden_gears(), available_ ).empty();
}

QByteArray weapon_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    BOOST_FOREACH( Gear g, available_ ){
        ds << (qint32)g;
    }
    
    return res;
}

void weapon_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version != 1)
        throw cl::except::error("Invalid filter version");

    available_.clear();
    
    BOOST_FOREACH( Gear g, forbidden( AllGear ) ){
        qint32 v;
        ds >> v;
        available_.insert( Gear(v) );
    }

    emit changed_signal();
}

void weapon_filter::insert(Gear g)
{
    available_.insert(g);
    emit changed_signal();
}

void weapon_filter::erase(Gear g)
{
    available_.erase(g);
    emit changed_signal();
}

const std::set< Gear >& weapon_filter::available() const
{ return available_; }


