#include <QRegExp>
#include <QObject>

#include "exception.h"
#include "server_id.h"

const QString c_port_incorrect = QObject::tr("Port incorrect");
const QString c_ip_incorrect = QObject::tr("IP incorrect");

server_id::server_id()
    : ip_(), host_name_(), port_(0)
{
}

server_id::server_id(const QString & ip, const QString & hostName, int port)
{
    set_ip(ip);
    set_host_name(hostName);
    set_port(port);
}

server_id::server_id(const QString & address)
{
    QRegExp rx("^((\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})|([^:]+)):(\\d{1,5})$");
    if (!(rx.exactMatch(address)))
        throw qexception(QObject::tr("Address syntax error"));
    set_ip(rx.cap(2));
    set_host_name(rx.cap(3));
    set_port(rx.cap(4));
}

bool operator ==(const server_id & a, const server_id & b)
{
    return a.host_name() == b.host_name() && a.ip() == b.ip() && a.port() == b.port();
}

bool operator <(const server_id & a, const server_id & b)
{
    if (a.host_name() == b.host_name())
    {
        if (a.ip() == b.ip())
            return a.port() < b.port();
        else
            return a.ip() < b.ip();
    } else
        return a.host_name() < b.host_name();
}

bool operator >(const server_id & a, const server_id & b)
{
    if (a.host_name() == b.host_name())
    {
        if (a.ip() == b.ip())
            return a.port() > b.port();
        else
            return a.ip() > b.ip();
    } else
        return a.host_name() > b.host_name();
}

QString server_id::address() const
{
    return QString("%1:%2").arg(ip_or_host()).arg(port_);
}

QString server_id::ip_or_host() const
{
    if (ip_.isEmpty())
        return host_name_;
    else
        return ip_;
}

// bool operator <=(const ServerID & a, const ServerID & b)
// {
// }
//
// bool operator >=(const ServerID & a, const ServerID & b)
// {
// }

void server_id::set_port(int val)
{
    if (val < 0 || val > 65535)
        throw qexception(QObject::tr("Port must be in range 0-65335"));
    port_ = val;
}

void server_id::set_port(const QString & val)
{
    if (val.isEmpty())
    {
        set_port(0);
        return;
    }
    bool ok = false;
    int port = val.toInt(&ok);
    if (!ok)
        throw qexception(c_port_incorrect);
    set_port(port);
}

void server_id::set_ip(const QString & val)
{
    if (!val.isEmpty())
    {
        QRegExp rx("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})");
        if (!rx.exactMatch(val))
            throw qexception(c_ip_incorrect);
    }
    ip_ = val;
}

void server_id::set_host_name(const QString & val)
{
    host_name_ = val;
}

bool server_id::is_empty()
{
    return host_name_.isEmpty() && ip_.isEmpty();
}



