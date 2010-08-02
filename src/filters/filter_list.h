#ifndef FILTER_LIST_H
#define	FILTER_LIST_H

#include <QString>
#include <map>

#include "pointers.h"

class server_info;

class filter_list
{
public:
    filter_list(filter_factory_p factory);

    /*! Filters factory. */
    filter_factory_p factory() const {return factory_;}

    /*! Filter list currently contained.  */
    filters_t filters() const {return filters_;}

    /*! Root filter. */
    filter_p root_filter() const {return root_filter_;}
    void set_root_filter(filter_p f);

    /*! Add new filter to list. Can be created via factory. */
    void add_filter(filter_p f);

    /*! Remove filter from list. */
    void delete_filter(filter_p f);

    /*! Take filter by his uid (name). Can be useful in metafilters. */
    filter_p get_filter_by_name(const QString& name) const;

private:
    filter_factory_p factory_;
    filters_t filters_;
    filter_p root_filter_;
};

#endif	/* FILTER_LIST_H */

