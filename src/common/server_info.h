#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <map>

#include <QIcon>
#include <QMetaType>

#include <common/server_id.h>
#include "player_info.h"
#include "../pointers.h"
#include "tools.h"

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
        s_up,
        s_down,
        s_error
    };

    server_info();

    server_id id;
    bool updating;
    status_t status;
    QString status_name() const;
    QString name;
    QString game_type; /* q3ut4 */
    player_info_list players;
    int max_player_count;
    QString country;
    QString country_code;
    game_mode mode;
    QString mode_name() const;
    static QString get_mode_name(game_mode mode);
    int ping;
    int retries;
    int red_score;
    int blue_score;
    QString map;
    QString map_url;

    const QString& get_info(const QString& key, const QString& default_value = QString()) const;

    /*! additional server info */
    typedef std::map<QString, QString> info_t;
    info_t info;

    /*! Info update stamp. Changed after each update | change. Initially it have value 1. */
    int update_stamp() const {return update_stamp_;}

    /*! Changes update stamp */
    void changed();

    /*! Correctly update from anoter server_info */
    void update_from(const server_info& src);

    /*! String contains all useful information about server. Primarily used for search. */
    const QString& meta_info_string() const;

    bool is_password_needed() const;
    bool is_pure() const;
    bool is_full() const;

    int private_slots() const;
    int public_slots() const;
    
    /*! Set of forbidden weapons */
    std::set<Gear> forbidden_gears() const;

private:
    mutable QString meta_info_string_;

    int update_stamp_;
};

Q_DECLARE_METATYPE(server_info_p)

typedef std::map<server_id, server_info_p> server_info_list;

#endif
