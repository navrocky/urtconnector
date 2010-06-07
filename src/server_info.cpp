#include <QObject>

#include "server_info.h"

server_info::server_info()
: updating(false)
, status(s_none)
, max_player_count(0)
, mode(gm_none)
, ping(0)
, retries(0)
, red_score(0)
, blue_score(0)
, update_stamp(0)
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

QString server_info::status_name()
{
    if (updating)
        return QObject::tr("Updating");
    
    switch (status)
    {
        case s_down: return QObject::tr("Offline");
        case s_up: return QObject::tr("Online");
    }
    return QObject::tr("Unknown");
}

QString server_info::get_info(const QString & key, const QString & default_value) const
{
    info_t::const_iterator it( info.find(key) );
    if ( it != info.end() )
        return it->second;
    else
        return default_value;
}

void server_info::update_from(const server_info& src)
{
    if (!src.name.isEmpty())
        name = src.name;
    if (!src.country.isEmpty())
        country = src.country;
    if (!src.country_code.isEmpty())
        country_code = src.country_code;
    if (!src.info.empty())
        info = src.info;
    if (!src.map.isEmpty())
        map = src.map;
    if (!src.map_url.isEmpty())
        map_url = src.map_url;
    if (src.mode != gm_none)
        mode = src.mode;
    if (src.ping != 0)
        ping = src.ping;
    if (!src.strict_name.isEmpty())
        strict_name = src.strict_name;
//    if (!src.players.empty())
    players = src.players;
    if (src.red_score != 0)
        red_score = src.red_score;
    if (src.retries != 0)
        retries = src.retries;
    status = src.status;
    if (src.blue_score != 0)
        blue_score = src.blue_score;
    if (!src.game_type.isEmpty())
        game_type = src.game_type;
    id = src.id;
    if (src.max_player_count != 0)
        max_player_count = src.max_player_count;
    update_stamp++;
}

//void server_info::update_from(const server_info& src)
//{}



