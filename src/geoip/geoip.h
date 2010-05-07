#ifndef URT_GEOIP_H
#define URT_GEOIP_H

#include <boost/shared_ptr.hpp>

#include <QString>
#include <QIcon>

/*! \brief class that wraps GeoIP C-interface
 */
class geoip
{
public:

    ///GeoIP database not used
    static const QString c_dummy_db;

    ///Creates geoip with opening \b database file.
    geoip(const QString& database = c_dummy_db);

    bool operator()()const;

    void set_database(const QString& database);

    QString code(const QString& addr) const;
    QString code3(const QString& addr) const;
    QString country(const QString& addr) const;

    /*! Returns flag file name by country code */
    static QString get_flag_filename_by_country(const QString& code);

    /*! Returns country flag by code */
    static QIcon get_flag_by_country(const QString& code);

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};

#endif
