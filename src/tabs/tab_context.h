#ifndef TAB_CONTEXT_H
#define	TAB_CONTEXT_H

#include <common/server_list.h>
#include <common/server_bookmark.h>
#include <filters/pointers.h>

class job_queue;
class geoip;
class QAction;

class tab_context
{
public:
    tab_context(server_list_p serv_list,
                filter_factory_p filter_factory,
                server_bookmark_list_p bookmarks,
                job_queue* que,
                geoip* geo,
                QAction* connect_action);

    server_list_p serv_list() const {return serv_list_;}
    filter_factory_p filter_factory() const {return filter_factory_;}
    server_bookmark_list_p bookmarks() const {return bookmarks_;}
    job_queue* job_que() const {return que_;}
    geoip* geo() const {return geo_;}
    QAction* connect_action() const {return connect_action_;}

private:
    server_list_p serv_list_;
    filter_factory_p filter_factory_;
    server_bookmark_list_p bookmarks_;
    job_queue* que_;
    geoip* geo_;
    QAction* connect_action_;
};

#endif	/* TAB_CONTEXT_H */

