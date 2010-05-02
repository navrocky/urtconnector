
#ifndef URT_GEOIP_H
#define URT_GEOIP_H

#include <boost/shared_ptr.hpp>

#include <QString>

/*! \brief class that wraps GeoIP C-interface
*/
class geoip{
public:

    ///GeoIP database not used
    static const QString DummyDB;


    ///Creates geoip with opening \b database file.
    geoip( const QString& database = DummyDB );

    ~geoip();

    bool operator()()const;
    
    void set_database( const QString& database );

    QString code( const QString& addr ) const;
    QString code3( const QString& addr ) const;
    QString country( const QString& addr ) const;

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};

#endif
