
#include <stdexcept>

#include <QObject>
#include <QRegExp>

#include "GeoIP_impl.h"
#include "geoip.h"

const QString geoip::c_dummy_db = "dummy";

//object that dispatching lifetime of the GeoIp pointer

struct geoip_disp
{
    GeoIP* ptr;

    geoip_disp(const QString & database)
    {
        //Dont use GEOIP_MMAP_CACHE because of it unimplemented under windows
        if (!(ptr = GeoIP_open(database.toLocal8Bit().data(), GEOIP_MEMORY_CACHE)))
            throw std::runtime_error(QObject::tr("Can't open GeoIP database %1").arg(database).toLocal8Bit().data());
    }

    ~geoip_disp()
    {
        GeoIP_delete(ptr);
    }
};

struct geoip::Pimpl
{

    Pimpl(const QString & database)
    : ip_rx("^[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}\\.[\\d]{1,3}$")
    {
        set_database(database);
    }

    bool is_ip(const QString& ip_or_host) const
    {
        return ip_rx.exactMatch(ip_or_host);
    }

    void set_database(const QString & database)
    {
        (database != geoip::c_dummy_db) ? gi.reset(new geoip_disp(database)) : gi.reset();
    }

    QString code(const QString & addr) const
    {
        if (!gi)
            return QString();
        if (is_ip(addr))
            return GeoIP_country_code_by_addr(gi->ptr, addr.toLocal8Bit().data());
        else
            return GeoIP_country_code_by_name(gi->ptr, addr.toLocal8Bit().data());
    }

    QString code3(const QString & addr) const
    {
        if (!gi)
            return QString();
        if (is_ip(addr))
            return GeoIP_country_code3_by_addr(gi->ptr, addr.toLocal8Bit().data());
        else
            return GeoIP_country_code3_by_name(gi->ptr, addr.toLocal8Bit().data());
    }

    QString country(const QString & addr) const
    {
        if (!gi)
            return QString();
        if (is_ip(addr))
            return GeoIP_country_name_by_addr(gi->ptr, addr.toLocal8Bit().data());
        else
            return GeoIP_country_name_by_name(gi->ptr, addr.toLocal8Bit().data());
    }

    boost::shared_ptr<geoip_disp> gi;
    QRegExp ip_rx;
};

geoip::geoip(const QString& database)
: p_(new Pimpl(database))
{
}

bool geoip::operator()()const
{
    return p_;
}

QString geoip::code(const QString& ip_or_host) const
{
    return p_->code(ip_or_host);
}

QString geoip::code3(const QString& ip_or_host) const
{
    return p_->code3(ip_or_host);
}

QString geoip::country(const QString& ip_or_host) const
{
    return p_->country(ip_or_host);
}

void geoip::set_database(const QString& database)
{
    p_->set_database(database);
}

QIcon geoip::get_flag_by_country(const QString& code)
{
    return QIcon(get_flag_filename_by_country(code));
}

QString geoip::get_flag_filename_by_country(const QString& code)
{
    return QString(":/flags/flags/%1.png").arg(code.toLower());
}
