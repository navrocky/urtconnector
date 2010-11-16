#ifndef _SERVER_LIST_H
#define _SERVER_LIST_H

#include "server_info.h"
#include "common/server_id.h"

/*! Server list, used by server_list_widget to fetching server_info. */
class server_list
{
public:
    server_list();

    /*! Resulting server info list */
    server_info_list& list() {return list_;}
    const server_info_list& list() const {return list_;}

    /*! List state. Changed always after list change. */
    int state() const {return state_;}

    /*! Call this after any list() change. */
    void change_state();

private:
    server_info_list list_;
    int state_;
};

#endif
