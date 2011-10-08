#include "tab_context.h"

tab_context::tab_context(server_list_p serv_list,
                         filter_factory_p filter_factory,
                         server_bookmark_list_p bookmarks,
                         job_queue* que,
                         geoip* geo,
                         QAction* connect_action,
                         tracking::manager* track_man,
                         tracking::context_p track_ctx,
                         friend_list* friends)
: serv_list_(serv_list)
, filter_factory_(filter_factory)
, bookmarks_(bookmarks)
, que_(que)
, geo_(geo)
, connect_action_(connect_action)
, track_man_(track_man)
, track_ctx_(track_ctx)
, friends_(friends)
{
}
