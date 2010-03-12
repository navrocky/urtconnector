#include <QRegExp>
#include <QObject>

#include "exception.h"
#include "serverid.h"

const QString c_port_incorrect = QObject::tr("Port incorrect");
const QString c_ip_incorrect = QObject::tr("IP incorrect");

server_id::server_id()
    : ip_(), hostName_(), port_(0)
{
}

server_id::server_id(const QString & ip, const QString & hostName, int port)
{
    setIp(ip);
    setHostName(hostName);
    setPort(port);
}

server_id::server_id(const QString & address)
{
    QRegExp rx("^((\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})|([^:]+)):(\\d{1,5})$");
    if (!(rx.exactMatch(address)))
        throw qexception(QObject::tr("Address syntax error"));
    setIp(rx.cap(2));
    setHostName(rx.cap(3));
    setPort(rx.cap(4));
}

server_id::server_id(const server_id & src)
    : ip_(src.ip()),
      hostName_(src.hostName()),
      port_(src.port())
{
}

server_id::~server_id()
{
}

bool operator ==(const server_id & a, const server_id & b)
{
    return a.hostName() == b.hostName() && a.ip() == b.ip() && a.port() == b.port();
}

bool operator <(const server_id & a, const server_id & b)
{
    if (a.hostName() == b.hostName())
    {
        if (a.ip() == b.ip())
            return a.port() < b.port();
        else
            return a.ip() < b.ip();
    } else
        return a.hostName() < b.hostName();
}

bool operator >(const server_id & a, const server_id & b)
{
    if (a.hostName() == b.hostName())
    {
        if (a.ip() == b.ip())
            return a.port() > b.port();
        else
            return a.ip() > b.ip();
    } else
        return a.hostName() > b.hostName();
}

QString server_id::address() const
{
    return QString("%1:%2").arg(ipOrHost()).arg(port_);
}

QString server_id::ipOrHost() const
{
    if (ip_.isEmpty())
        return hostName_;
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

void server_id::setPort(int val)
{
    if (val < 0 || val > 65535)
        throw qexception(QObject::tr("Port must be in range 0-65335"));
    port_ = val;
}

void server_id::setPort(const QString & val)
{
    if (val.isEmpty())
    {
        setPort(0);
        return;
    }
    bool ok = false;
    int port = val.toInt(&ok);
    if (!ok)
        throw qexception(c_port_incorrect);
    setPort(port);
}

void server_id::setIp(const QString & val)
{
    if (!val.isEmpty())
    {
        QRegExp rx("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})");
        if (!rx.exactMatch(val))
            throw qexception(c_ip_incorrect);
    }
    ip_ = val;
}

void server_id::setHostName(const QString & val)
{
    hostName_ = val;
}

bool server_id::isEmpty()
{
    return hostName_.isEmpty() && ip_.isEmpty();
}



