#ifndef FILTER_LIST_H
#define	FILTER_LIST_H

#include <QObject>
#include <QString>
#include <map>

#include "pointers.h"

class server_info;

class filter_list : public QObject
{
    Q_OBJECT
public:
    filter_list(filter_factory_p factory);

    /*! Filters factory. */
    filter_factory_p factory() const {return factory_;}

    /*! Filter list currently contained.  */
    filters_t filters() const {return filters_;}

    /*! Root filter. */
    filter_p root_filter() const {return root_filter_;}
    void set_root_filter(filter_p f);

    filter_p create_by_class_id(const QString& id);

    /*! Add new filter to list. Can be created via factory. */
    void add_filter(filter_p f);

    /*! Remove filter from list. */
    void delete_filter(filter_p f);

    /*! Take filter by his uid (name). Can be useful in metafilters. */
    filter_p get_filter_by_name(const QString& name) const;

    /*! Filter selected for list toolbar. */
    filter_weak_p toolbar_filter() const {return toolbar_filter_;}

    /*! Change toolbar filter. */
    void set_toolbar_filter(filter_weak_p f);

signals:
    void toolbar_filter_changed();

private:
    filter_factory_p factory_;
    filters_t filters_;
    filter_p root_filter_;
    filter_weak_p toolbar_filter_;
};

#endif	/* FILTER_LIST_H */

