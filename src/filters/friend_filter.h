#ifndef FRIEND_FILTER_H
#define	FRIEND_FILTER_H

#include <QString>
#include <QHash>
#include <QRegExp>

#include <common/server_info.h>
#include "pointers.h"
#include "filter.h"

class friend_filter_class : public filter_class
{
public:
    friend_filter_class();
    virtual filter_p create_filter();
    static const char* get_id();
};

class friend_filter : public filter
{
    Q_OBJECT
public:
    friend_filter(filter_class_p fc);
    virtual bool filter_server(const server_info& si, filter_context& ctx);

private:
    typedef QHash<QString, QRegExp> regexps_t;
    regexps_t regexps_;
};

#endif	/* friend_filter_H */
