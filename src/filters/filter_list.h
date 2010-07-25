#ifndef FILTER_LIST_H
#define	FILTER_LIST_H

#include <QString>
#include <map>

#include "pointers.h"

class server_info;

class filter_list
{
public:

    /*! Filter server info. */
    bool filter_server(const server_info& si);

    /*! Filters factory. */
    filter_factory_p factory() const {return factory_;}

    /*! Filter list currently contained.  */
    filters_t filters() const;

    /*! Add new filter to list. Can be created via factory. */
    void add_filter(filter_p f);

    /*! Remove filter from list. */
    void delete_filter(filter_p f);

    /*! Is filter enabled? */
    bool is_filter_enabled(filter_p f) const;

    /*! Enables filter. */
    void set_filter_enabled(filter_p f, bool enabled);

    /*! Take filter by his uid (name). Can be useful in metafilters. */
    filter_p get_filter_by_uid(const QString& uid) const;

private:
    struct filter_rec
    {
        filter_rec();
        bool enabled;
    };
    typedef std::map<filter_p, filter_rec> filter_recs_t;

    filter_factory_p factory_;
    filter_recs_t filters_;
};

#endif	/* FILTER_LIST_H */

