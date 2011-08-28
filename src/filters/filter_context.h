#ifndef FILTER_CONTEXT_H
#define	FILTER_CONTEXT_H

#include <QMap>

class filter_context
{
public:
    typedef QMap<QString, QString> data_map_t;
    
    filter_context()
    : data(0)
    , full_filter_process(false)
    {}

    data_map_t* data;
    bool full_filter_process;
};

#endif	/* FILTER_CONTEXT_H */

