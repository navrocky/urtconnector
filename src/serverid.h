#ifndef SERVERID_H
#define SERVERID_H

#include <QString>

/*! \brief Server ID.

    Unique server identificator.

    \author Navrocky Vladislaw (navrocky@visualdata.ru)
*/
class ServerID
{
public:
    ServerID();
    ServerID(const QString& ip, const QString& hostName, int port);

    /*! use address string, ex.: "myhostname:port" */
    ServerID(const QString& address);
    
    ServerID(const ServerID& src);
    ~ServerID();

    QString ip() const {return ip_;}
    QString hostName() const {return hostName_;}
    int port() const {return port_;}

    QString address();

private:
    QString ip_;
    QString hostName_;
    int port_;
};

bool operator==(const ServerID& a, const ServerID& b);
bool operator<(const ServerID& a, const ServerID& b);
bool operator>(const ServerID& a, const ServerID& b);
// bool operator<=(const ServerID& a, const ServerID& b);
// bool operator>=(const ServerID& a, const ServerID& b);

#endif
