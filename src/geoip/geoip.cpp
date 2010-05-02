
#include <stdexcept>

#include <QIcon>
#include <QHash>

#include "GeoIP_impl.h"
#include "geoip.h"

#include <iostream>

typedef QHash<QString, QIcon> icons_t;

const QString geoip::DummyDB = "dummy";

//object that dispatching lifetime of the GeoIp pointer
struct geoip_disp{
    GeoIP* ptr;

    geoip_disp(const QString& database)
    {
        if ( ! (ptr =  GeoIP_open( database.toLocal8Bit().data(), GEOIP_MMAP_CACHE )) )
            throw std::runtime_error( QString("Can't open GeoIP database %1").arg(database).toLocal8Bit().data() );
    }

    ~geoip_disp()
    { GeoIP_delete(ptr); }

};

struct geoip::Pimpl{
    Pimpl(const QString& database)
    { set_database(database); }

    void set_database(const QString& database)
    {
        ( database != geoip::DummyDB ) ? gi.reset( new geoip_disp(database) ) : gi.reset();
        icons.clear();
    }

    QString code( const QString& addr ) const
    { return (gi) ? GeoIP_country_code_by_addr(  gi->ptr, addr.toLocal8Bit().data() ) : QString(); }

    QString code3( const QString& addr ) const
    { return (gi) ? GeoIP_country_code3_by_addr(  gi->ptr, addr.toLocal8Bit().data() ): QString(); }

    QString country( const QString& addr ) const
    { return (gi) ? GeoIP_country_name_by_addr(  gi->ptr, addr.toLocal8Bit().data() ): QString(); }

    QString flag_path( const QString& addr ) const
    { return (gi) ? QString(":/flags/flags/%1.png").arg( code(addr).toLower() ): QString(); }

    const QIcon& flag( const QString& addr )
    {
        if ( !gi ) return dummy_icon;

        if ( !icons.contains( addr ) )
            icons[addr] = QPixmap( flag_path(addr) );
        
        return icons[addr];
    }

    boost::shared_ptr<geoip_disp>  gi;
    icons_t icons;
    QIcon dummy_icon;
    
};

geoip::geoip( const QString& database )
    : p_( new Pimpl(database) )
{}

geoip::~geoip()
{}

bool geoip::operator()()const
{ return p_; }


QString geoip::code( const QString& addr ) const
{ return p_->code(addr); }

QString geoip::code3( const QString& addr ) const
{ return p_->code3(addr); }

QString geoip::country( const QString& addr ) const
{ return p_->country(addr); }

const QIcon& geoip::flag( const QString& addr ) const
{ return p_->flag(addr); }

QString geoip::flag_path( const QString& addr ) const
{ return p_->flag_path(addr); }

void geoip::set_database( const QString& database )
{ p_->set_database(database); }








