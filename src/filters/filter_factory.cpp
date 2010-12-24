#include <assert.h>

#include "filter.h"

#include "filter_factory.h"

void filter_factory::add_class(filter_class_p fc)
{
    classes_[fc->id()] = fc;
}

bool filter_factory::has_filter(const QString& id) const
{
    return classes_.find(id) != classes_.end() ;
}


filter_p filter_factory::create_filter_by_id(const QString& id) const
{
    filter_classes_map_t::const_iterator it = classes_.find(id);
    assert(it != classes_.end());
    return it->second->create_filter();
}

filter_factory::filter_classes_t filter_factory::filter_classes()
{
    filter_classes_t res;
    for (filter_classes_map_t::iterator it = classes_.begin(); it != classes_.end(); it++)
        res.push_back(it->second);
    return res;
}
