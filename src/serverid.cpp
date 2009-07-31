#include "serverid.h"

ServerID::ServerID()
    : ip_(), hostName_(), port_(0)
{
}

ServerID::ServerID(const QString & ip, const QString & hostName, int port)
    : ip_(ip), hostName_(hostName), port_(port)
{
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
    if ( a.hostName().isEmpty() )
        return a.ip() == b.ip() && a.port() == b.port();
    else
        return a.hostName() == b.hostName() && a.port() == b.port();
}

bool operator <(const ServerID & a, const ServerID & b)
{
    if ( a.hostName().isEmpty() )
    {
        if (a.ip() == b.ip()) return a.port() < b.port();
            else return a.ip() < b.ip();
    } else
    {
        if (a.hostName() == b.hostName()) return a.port() < b.port();
            else return a.hostName() < b.hostName();
    }
}

bool operator >(const ServerID & a, const ServerID & b)
{
    if ( a.hostName().isEmpty() )
    {
        if (a.ip() == b.ip()) return a.port() > b.port();
            else return a.ip() > b.ip();
    } else
    {
        if (a.hostName() == b.hostName()) return a.port() > b.port();
            else return a.hostName() > b.hostName();
    }
}

// bool operator <=(const ServerID & a, const ServerID & b)
// {
// }
// 
// bool operator >=(const ServerID & a, const ServerID & b)
// {
// }


