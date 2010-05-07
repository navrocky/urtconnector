#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <map>

#include <QIcon>
#include <QMetaType>

#include "server_id.h"
#include "player_info.h"

class server_info
{
public:
    enum game_mode
    {
        gm_none = 0,
        gm_free_for_all = 1,
        gm_team_death_match = 4,
        gm_team_survivor = 5,
        gm_follow_the_leader = 6,
        gm_capture_and_hold = 7,
        gm_capture_the_flag = 8,
        gm_bomb_mode = 9
    };

    enum status_t
    {
        s_none = 0,
        s_updating,
        s_up,
        s_down
    };

    server_info();

    server_id id;
    status_t status;
    QString name;
    QString game_type; /* q3ut4 */
    player_info_list players;
    int max_player_count;
    QString country;
    QString country_code;
    game_mode mode;
    QString mode_name() const;
    int ping;
    int retries;
    int red_score;
    int blue_score;
    QString map;
    QString map_url;

    QString get_info(const QString& key) const;

    /*! additional server info */
    typedef std::map<QString, QString> info_t;
    info_t info;

    /*! Info update stamp. Changed after each update. */
    int update_stamp;

    /*! Correctly update from anoter server_info */
    void update_from(const server_info& src);
private:

};

//qRegisterMetaType<server_info>("server_info");
Q_DECLARE_METATYPE(server_info);

typedef std::map<server_id, server_info> server_info_list;

#endif
