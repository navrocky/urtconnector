#include "composite_filter.h"

composite_filter::composite_filter(filter_class_p fc)
: filter(fc)
{
}

bool composite_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    switch (operation_)
    {
        case op_and:
        {
            foreach (filter_p f, filters_)
                if (!f->filter_server(si))
                    return false;
            return true;
        }
        case op_or:
        {
            foreach (filter_p f, filters_)
                if (f->filter_server(si))
                    return true;
            return filters_.size() > 0;
        }

        default:;
    }
    return true;
}

void composite_filter::set_opertaion(operation_t op)
{
    operation_ = op;
}

void composite_filter::add_filter(filter_p f)
{
    filters_.push_back(f);
}

void composite_filter::remove_filter(filter_p f)
{
    filters_t::iterator it = std::find(filters_.begin(), filters_.end(), f);
    assert(it != filters_.end());
    filters_.erase(it);
}

