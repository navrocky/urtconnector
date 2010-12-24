#ifndef FILTER_FACTORY_H
#define	FILTER_FACTORY_H

#include <vector>
#include <map>
#include <QString>

#include "pointers.h"

class filter_factory
{
public:
    typedef std::vector<filter_class_p> filter_classes_t;

    void add_class(filter_class_p fc);

    bool has_filter( const QString& id ) const;
    
    filter_p create_filter_by_id(const QString& id) const;

    filter_classes_t filter_classes();

private:
    typedef std::map<QString, filter_class_p> filter_classes_map_t;

    filter_classes_map_t classes_;
};

#endif	/* FILTER_FACTORY_H */

