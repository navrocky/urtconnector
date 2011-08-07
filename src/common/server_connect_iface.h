#ifndef CONNECT_IFACE_H
#define	CONNECT_IFACE_H

class server_connect_iface
{
public:
    virtual void connect_to_server(const server_id& id,
                                   const QString& player_name,
                                   const QString& password,
                                   bool check_before_connect) = 0;
};

#endif

