#include "context.h"

namespace tracking
{

context_t::context_t(const server_list_p& srv_list_,
                     update_dispatcher* update_dispatcher_,
                     QSystemTrayIcon* tray_,
                     server_connect_iface* connect_iface_,
                     select_server_func_t select_server_)
: srv_list(srv_list_)
, update_disp(update_dispatcher_)
, tray(tray_)
, connect_iface(connect_iface_)
, select_server(select_server_)
{
}

}