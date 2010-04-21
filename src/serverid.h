#ifndef SERVERID_H
#define SERVERID_H

#include <vector>
#include <QString>

/*! \brief Server ID.

    Unique server identificator.

    \author Navrocky Vladislaw (navrocky@visualdata.ru)
*/
class server_id
{
public:
    server_id();
    server_id(const QString& ip, const QString& hostName, int port);

    /*! use address string, ex.: "myhostname:port" */
    server_id(const QString& address);

    server_id(const server_id& src);
    ~server_id();

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

bool operator==(const server_id& a, const server_id& b);
bool operator<(const server_id& a, const server_id& b);
bool operator>(const server_id& a, const server_id& b);
// bool operator<=(const ServerID& a, const ServerID& b);
// bool operator>=(const ServerID& a, const ServerID& b);
typedef std::vector<server_id> server_id_list;

#endif
