#ifndef FILTER_CONTEXT_H
#define	FILTER_CONTEXT_H

#include <QMap>

class friend_list;

class filter_context
{
public:
    typedef QMap<QString, QString> data_map_t;

    filter_context(bool full_filter_process_,
                   friend_list* friends_)
    : data(0)
    , full_filter_process(full_filter_process_)
    , friends(friends_)
    {}

    data_map_t* data;
    bool full_filter_process;
    friend_list* friends;
};

#endif	/* FILTER_CONTEXT_H */

