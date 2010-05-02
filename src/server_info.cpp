#include <QObject>

#include "server_info.h"

server_info::server_info()
  : status(s_none),
    max_player_count(0),
    mode(gm_none),
    ping(0),
    retries(0),
    red_score(0),
    blue_score(0),
    update_stamp(0)
{
}

QString server_info::mode_name() const
{
    switch (mode)
    {
        case gm_none: return QString();
        case gm_free_for_all: return QObject::tr("Free For All");
        case gm_team_death_match: return QObject::tr("Team DeathMatch");
        case gm_team_survivor: return QObject::tr("Team Survivor");
        case gm_follow_the_leader: return QObject::tr("Follow The Leader");
        case gm_capture_and_hold: return QObject::tr("Capture And Hold");
        case gm_capture_the_flag: return QObject::tr("Capture The Flag");
        case gm_bomb_mode: return QObject::tr("Bomb Mode");
    }
    return QObject::tr("Unknown");
}

QString server_info::get_info(const QString& key) const
{
    info_t::const_iterator it( info.find(key) );
    if ( it != info.end() )
        return it->second;
    else
        return QString();
}



