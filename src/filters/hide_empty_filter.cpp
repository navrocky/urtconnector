#include <QObject>

#include "hide_empty_filter.h"

////////////////////////////////////////////////////////////////////////////////
// hide_empty_filter_class

hide_empty_filter_class::hide_empty_filter_class()
: filter_class(get_id(), QObject::tr("Hide empty servers"),
               QObject::tr("Hides all empty servers."))
{}

const char* hide_empty_filter_class::get_id()
{
    return "hide_empty";
}

filter_p hide_empty_filter_class::create_filter()
{
    return filter_p(new hide_empty_filter(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// hide_empty_filter

hide_empty_filter::hide_empty_filter(filter_class_p fc)
: filter(fc)
{
}

bool hide_empty_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    return si.players.size() > 0 || si.status != server_info::s_up;
}
