#include <QObject>
#include <boost/make_shared.hpp>

#include "hide_full_filter.h"

////////////////////////////////////////////////////////////////////////////////
// hide_full_filter_class

hide_full_filter_class::hide_full_filter_class()
: filter_class(get_id(), QObject::tr("Hide full servers"),
               QObject::tr("Hides all full servers."))
{}

const char* hide_full_filter_class::get_id()
{
    return "hide_full";
}

filter_p hide_full_filter_class::create_filter()
{
    return boost::make_shared<hide_full_filter>(shared_from_this());
}

////////////////////////////////////////////////////////////////////////////////
// hide_full_filter

hide_full_filter::hide_full_filter(filter_class_p fc)
: filter(fc)
{
}

bool hide_full_filter::filter_server(const server_info& si)
{
    if (!enabled())
        return true;

    int private_slots = si.get_info("sv_privateClients").toInt();
    return si.players.size() < si.max_player_count - private_slots;
}
