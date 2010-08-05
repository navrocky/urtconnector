#include <QObject>
#include <boost/make_shared.hpp>

#include "composite_filter.h"

////////////////////////////////////////////////////////////////////////////////
// composite_filter_class

composite_filter_class::composite_filter_class()
: filter_class(get_id(), QObject::tr("Composite filter"),
               QObject::tr("Composite child filters using logical operations."))
{}

const char* composite_filter_class::get_id()
{
    return "composite";
}

filter_p composite_filter_class::create_filter()
{
    return boost::make_shared<composite_filter>(shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
// composite_filter

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

