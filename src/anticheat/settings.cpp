#include <QSettings>

#include "settings.h"

namespace anticheat
{

int settings::interval() const
{
    return part()->value("interval", 30).toInt();
}

void settings::set_interval(int val)
{
    part()->setValue("interval", val);
}

bool settings::use_ftp() const
{
    return part()->value("use_ftp", true).toBool();
}

void settings::set_use_ftp(bool val)
{
    part()->setValue("use_ftp", val);
}

server_id settings::ftp_address() const
{
    QString s = part()->value("ftp_address", QString()).toString();
    if (s.isEmpty())
        return server_id();
    else
        return server_id(s, 21);
}

void settings::set_ftp_address(const server_id& val)
{
    part()->setValue("ftp_address", val.address());
}

QString settings::ftp_login() const
{
    return part()->value("ftp_login", QString()).toString();
}

void settings::set_ftp_login(const QString& val)
{
    part()->setValue("ftp_login", val);
}

QString settings::ftp_password() const
{
    return part()->value("ftp_password", QString()).toString();
}

void settings::set_ftp_password(const QString& val)
{
    part()->setValue("ftp_password", val);
}

QString settings::ftp_folder() const
{
    return part()->value("ftp_folder", "screenshots").toString();
}

void settings::set_ftp_folder(const QString& val)
{
    part()->setValue("ftp_folder", val);
}

bool settings::use_local_folder() const
{
    return part()->value("use_local_folder", false).toBool();
}

void settings::set_use_local_folder(bool val)
{
    part()->setValue("use_local_folder", val);
}

QString settings::local_folder() const
{
    return part()->value("local_folder", "screenshots").toString();
}

void settings::set_local_folder(const QString& val)
{
    part()->setValue("local_folder", val);
}

}
