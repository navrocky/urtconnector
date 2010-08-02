#include <algorithm>

#include "filter.h"

#include "filter_list.h"

////////////////////////////////////////////////////////////////////////////////
// filter_list

filter_list::filter_list(filter_factory_p factory)
: factory_(factory)
{
}

void filter_list::add_filter(filter_p f)
{
    filters_.push_back(f);
}

void filter_list::delete_filter(filter_p f)
{
    filters_t::iterator it = std::find(filters_.begin(), filters_.end(), f);
    assert(it != filters_.end());
    filters_.erase(it);
}

filter_p filter_list::get_filter_by_name(const QString& name) const
{
    foreach (filter_p f, filters_)
        if (f->name() == name)
            return f;
    return filter_p();
}

void filter_list::set_root_filter(filter_p f)
{
    root_filter_ = f;
}

