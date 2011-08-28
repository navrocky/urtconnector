#ifndef TRACKING_CONTEXT_H
#define	TRACKING_CONTEXT_H

#include <QMap>
#include <QString>
#include <boost/function.hpp>
#include "../pointers.h"

class update_dispatcher;
class server_connect_iface;
class QSystemTrayIcon;
class server_id;

namespace tracking
{

typedef QMap<QString, QString> data_map_t;
typedef boost::function<void (const server_id&)> select_server_func_t;

class context_t
{
public:
    context_t(const server_list_p& srv_list,
              update_dispatcher* update_dispatcher,
              QSystemTrayIcon* tray,
              server_connect_iface* connect_iface,
              select_server_func_t select_server);

    server_list_p srv_list;
    update_dispatcher* update_disp;
    data_map_t data;
    QSystemTrayIcon* tray;
    server_connect_iface* connect_iface;
    select_server_func_t select_server;
};

}

#endif

