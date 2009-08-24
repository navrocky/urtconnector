#ifndef SERVERID_H
#define SERVERID_H

#include <vector>
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
    QString ipOrHost() const;

    int port() const {return port_;}

    void setIp(const QString& val);
    void setHostName(const QString& val);
    void setPort(int val);
    void setPort(const QString& val);

    QString address() const;

    bool isEmpty();

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
typedef std::vector<ServerID> ServerIDList;

#endif
