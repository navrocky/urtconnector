#ifndef TRACKING_CONTEXT_H
#define	TRACKING_CONTEXT_H

#include <QMap>
#include <QString>
#include "../pointers.h"

class update_dispatcher;
class server_connect_iface;
class QSystemTrayIcon;

namespace tracking
{

typedef QMap<QString, QString> data_map_t;

class context_t
{
public:
    context_t(const server_list_p& srv_list,
              update_dispatcher* update_dispatcher,
              QSystemTrayIcon* tray,
              server_connect_iface* connect_iface);

    server_list_p srv_list;
    update_dispatcher* update_disp;
    data_map_t data;
    QSystemTrayIcon* tray;
    server_connect_iface* connect_iface;
};

}

#endif

