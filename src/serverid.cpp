#include <QRegExp>
#include <QObject>

#include "exception.h"
#include "serverid.h"

const QString c_port_incorrect = QObject::tr("Port incorrect");
const QString c_ip_incorrect = QObject::tr("IP incorrect");

ServerID::ServerID()
    : ip_(), hostName_(), port_(0)
{
}

ServerID::ServerID(const QString & ip, const QString & hostName, int port)
{
    setIp(ip);
    setHostName(hostName);
    setPort(port);
}

ServerID::ServerID(const QString & address)
{
    QRegExp rx("^((\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})|([^:]+)):(\\d{1,5})$");
    if (!(rx.exactMatch(address)))
        throw Exception(QObject::tr("Address syntax error"));
    setIp(rx.cap(2));
    setHostName(rx.cap(3));
    setPort(rx.cap(4));
}

ServerID::ServerID(const ServerID & src)
    : ip_(src.ip()),
      hostName_(src.hostName()),
      port_(src.port())
{
}

ServerID::~ServerID()
{
}

bool operator ==(const ServerID & a, const ServerID & b)
{
    return a.hostName() == b.hostName() && a.ip() == b.ip() && a.port() == b.port();
}

bool operator <(const ServerID & a, const ServerID & b)
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

bool operator >(const ServerID & a, const ServerID & b)
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

QString ServerID::address() const
{
    return QString("%1:%2").arg(ipOrHost()).arg(port_);
}

QString ServerID::ipOrHost() const
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

void ServerID::setPort(int val)
{
    if (val < 0 || val > 65535)
        throw Exception(QObject::tr("Port must be in range 0-65335"));
    port_ = val;
}

void ServerID::setPort(const QString & val)
{
    if (val.isEmpty())
    {
        setPort(0);
        return;
    }
    bool ok = false;
    int port = val.toInt(&ok);
    if (!ok)
        throw Exception(c_port_incorrect);
    setPort(port);
}

void ServerID::setIp(const QString & val)
{
    if (!val.isEmpty())
    {
        QRegExp rx("(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})\\.(\\d{1,3})");
        if (!rx.exactMatch(val))
            throw Exception(c_ip_incorrect);
    }
    ip_ = val;
}

void ServerID::setHostName(const QString & val)
{
    hostName_ = val;
}

bool ServerID::isEmpty()
{
    return hostName_.isEmpty() && ip_.isEmpty();
}



