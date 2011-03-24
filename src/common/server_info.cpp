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
, update_stamp_(1)
{
}

QString server_info::get_mode_name(game_mode mode)
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

QString server_info::mode_name() const
{
    return get_mode_name(mode);
}

QString server_info::status_name() const
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

const QString& server_info::get_info(const QString & key, const QString & default_value) const
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
    changed();
    meta_info_string_ = QString();
}

const QString& server_info::meta_info_string() const
{
    if (meta_info_string_.isEmpty())
    {
        QStringList pl;
        foreach (const player_info& pi, players)
            pl += pi.nick_name();

        meta_info_string_ = QString("%1 %2 %3 %4 %5 %6 %7").arg(name)
            .arg(id.address()).arg(country).arg(map).arg(mode_name()).arg(pl.join(" "))
            .arg(status_name());

    }
    return meta_info_string_;
}

bool server_info::is_password_needed() const
{
    return get_info("g_needpass").toInt();
}


bool server_info::is_pure() const
{
    return get_info("pure", "-1").toInt() == 0;
}

bool server_info::is_full() const
{
    return ( max_player_count > 0 ) && ( players.size() >= max_player_count - private_slots() );
}

int server_info::private_slots() const
{
    return get_info("sv_privateClients").toInt();
}

int server_info::public_slots() const
{
    return max_player_count - private_slots();
}

void server_info::changed()
{
    update_stamp_++;
}


