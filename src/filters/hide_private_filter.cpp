#include <QObject>

#include "hide_private_filter.h"

////////////////////////////////////////////////////////////////////////////////
// hide_private_filter_class

hide_private_filter_class::hide_private_filter_class()
: filter_class(get_id(), QObject::tr("Hide private servers"),
               QObject::tr("Hides all private servers."))
{}

const char* hide_private_filter_class::get_id()
{
    return "hide_private";
}

filter_p hide_private_filter_class::create_filter()
{
    return filter_p(new hide_private_filter(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// hide_private_filter

hide_private_filter::hide_private_filter(filter_class_p fc)
: filter(fc)
{
}

bool hide_private_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    return si.get_info("g_needpass").toInt() == 0;
}
