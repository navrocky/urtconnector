#ifndef SERVER_ID_H
#define SERVER_ID_H

#include <vector>
#include <QString>
#include <QSharedData>

#include "implicit_sharing.h"

/*! \brief Server ID.

    Unique server identificator.
*/
//    class impl : public QSharedData
//    {
//    public:
//        impl():port_(0){}
//        impl(const impl& src):QSharedData(src), ip_(src.ip_),host_name_(src.host_name_), port_(src.port_){}
//        ~impl(){}
//        QString ip_;
//        QString host_name_;
//        int port_;
//    };
class server_id
{
public:
    server_id();
    server_id(const QString& ip, const QString& host_name, int port);

    /*! Use address string, ex.: "myhostname:port" */
    server_id(const QString& address, int default_port = 27960);

    QString ip() const {return d->ip_;}
    QString host_name() const {return d->host_name_;}
    QString ip_or_host() const;

    int port() const {return d->port_;}

    void set_ip(const QString& val);
    void set_host_name(const QString& val);
    void set_port(int val);
    void set_port(const QString& val);

    /*! Returns server address in format "myhostname:port" */
    QString address() const;

    /*! Is empty address? True for default constructor result. */
    bool is_empty() const;

private:
    struct impl
    {
        QString ip_;
        QString host_name_;
        int port_;
    };
    implicit_sharing<impl> d;
};

bool operator==(const server_id& a, const server_id& b);
bool operator!=(const server_id& a, const server_id& b);
bool operator<(const server_id& a, const server_id& b);
bool operator>(const server_id& a, const server_id& b);
// bool operator<=(const ServerID& a, const ServerID& b);
// bool operator>=(const ServerID& a, const ServerID& b);
typedef std::vector<server_id> server_id_list;

#endif
