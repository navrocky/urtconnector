
#include <stdexcept>

#include <QObject>

#include "GeoIP_impl.h"
#include "geoip.h"

const QString geoip::DummyDB = "dummy";

//object that dispatching lifetime of the GeoIp pointer
struct geoip_disp{
    GeoIP* ptr;

    geoip_disp(const QString& database)
    {
        if ( ! (ptr =  GeoIP_open( database.toLocal8Bit().data(), GEOIP_MMAP_CACHE )) )
            throw std::runtime_error( QObject::tr("Can't open GeoIP database %1").arg(database).toLocal8Bit().data() );
    }

    ~geoip_disp()
    { GeoIP_delete(ptr); }

};

struct geoip::Pimpl{
    Pimpl(const QString& database)
    { set_database(database); }

    void set_database(const QString& database)
    { ( database != geoip::DummyDB ) ? gi.reset( new geoip_disp(database) ) : gi.reset(); }

    QString code( const QString& addr ) const
    { return (gi) ? GeoIP_country_code_by_addr(  gi->ptr, addr.toLocal8Bit().data() ) : QString(); }

    QString code3( const QString& addr ) const
    { return (gi) ? GeoIP_country_code3_by_addr(  gi->ptr, addr.toLocal8Bit().data() ): QString(); }

    QString country( const QString& addr ) const
    { return (gi) ? GeoIP_country_name_by_addr(  gi->ptr, addr.toLocal8Bit().data() ): QString(); }

    boost::shared_ptr<geoip_disp>  gi;
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

void geoip::set_database( const QString& database )
{ p_->set_database(database); }









