#include "friend_filter.h"

#include <cl/except/error.h>

#include <friend_list/friend_list.h>
#include <friend_list/friend_record.h>

#include "tools.h"

static const char* c_nick_name = "nickname";

////////////////////////////////////////////////////////////////////////////////
// friend_filter_class

friend_filter_class::friend_filter_class()
: filter_class(get_id(), QObject::tr("Find friends"),
               QObject::tr("Finds the server with you friends."))
{}

const char* friend_filter_class::get_id()
{
    return "friend";
}

filter_p friend_filter_class::create_filter()
{
    return filter_p(new friend_filter(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// friend_filter

friend_filter::friend_filter(filter_class_p fc)
: filter(fc)
{
}

bool friend_filter::filter_server(const server_info& si, filter_context& ctx)
{
    if (!enabled())
        return true;

    foreach(const friend_record& fr, ctx.friends->list())
    {
        if (fr.expression().isEmpty())
        {

            foreach(const player_info& pi, si.players)
            {
                if (fr.nick_name().compare(pi.nick_name().trimmed(), Qt::CaseInsensitive) == 0)
                {
                    if (ctx.data)
                        ctx.data->insert(c_nick_name, pi.nick_name());
                    return true;
                }
            }
        }
        else
        {
            regexps_t::iterator it = regexps_.find(fr.expression());
            if (it == regexps_.end())
                it = regexps_.insert(fr.expression(), QRegExp(fr.expression(), Qt::CaseInsensitive));
            QRegExp& rx = it.value();

            foreach(const player_info& pi, si.players)
            {
                if (rx.indexIn(pi.nick_name()) >= 0)
                {
                    if (ctx.data)
                        ctx.data->insert(c_nick_name, pi.nick_name());
                    return true;
                }
            }
        }
    }
    return false;
}
