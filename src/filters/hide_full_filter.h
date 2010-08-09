#ifndef HIDE_FULL_FILTER_H
#define	HIDE_FULL_FILTER_H

#include "pointers.h"
#include "filter.h"
#include "../server_info.h"

class hide_full_filter_class : public filter_class
{
public:
    hide_full_filter_class();
    virtual filter_p create_filter();
    static const char* get_id();
};

class hide_full_filter : public filter
{
    Q_OBJECT
public:
    hide_full_filter(filter_class_p fc);
    virtual bool filter_server(const server_info& si);
};


#endif	/* HIDE_FULL_FILTER_H */

