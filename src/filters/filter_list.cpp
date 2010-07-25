#include "filter.h"

#include "filter_list.h"

filter_list::filter_rec::filter_rec()
: enabled(true)
{}

////////////////////////////////////////////////////////////////////////////////
// filter_list

bool filter_list::filter_server(const server_info& si)
{
    for (filter_recs_t::iterator it = filters_.begin(); it != filters_.end(); ++it)
        if (it->second.enabled)
            if (!it->first->filter_server(si))
                return false;
    return true;
}

filters_t filter_list::filters() const
{
    filters_t res;
    for (filter_recs_t::const_iterator it = filters_.begin(); it != filters_.end(); ++it)
        res.push_back(it->first);
    return res;
}

void filter_list::add_filter(filter_p f)
{
    filters_[f] = filter_rec();
}

void filter_list::delete_filter(filter_p f)
{
    filter_recs_t::iterator it = filters_.find(f);
    assert(it != filters_.end());
    filters_.erase(it);
}

bool filter_list::is_filter_enabled(filter_p f) const
{
    filter_recs_t::const_iterator it = filters_.find(f);
    if (it != filters_.end())
        return it->second.enabled;
    else
        return false;
}

void filter_list::set_filter_enabled(filter_p f, bool enabled)
{
    filters_[f].enabled = enabled;
}

filter_p filter_list::get_filter_by_uid(const QString& uid) const
{
    for (filter_recs_t::const_iterator it = filters_.begin(); it != filters_.end(); ++it)
        if (it->first->uid() == uid)
            return it->first;
    return filter_p();
}

