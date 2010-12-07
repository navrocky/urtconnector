#ifndef HIDE_EMPTY_FILTER_H
#define	HIDE_EMPTY_FILTER_H

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>

class hide_empty_filter_class : public filter_class
{
public:
    hide_empty_filter_class();
    virtual filter_p create_filter();
    static const char* get_id();
};

class hide_empty_filter : public filter
{
    Q_OBJECT
public:
    hide_empty_filter(filter_class_p fc);
    virtual bool filter_server(const server_info& si);
};

#endif	/* HIDE_EMPTY_FILTER_H */

