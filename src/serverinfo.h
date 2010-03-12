#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <map>

#include "serverid.h"
#include "playerinfo.h"

class server_info
{
public:
    enum game_mode
    {
        None = 0,
        FreeForAll = 1,
        TeamDeathMatch = 4,
        TeamSurvivor = 5,
        FollowTheLeader = 6,
        CaptureAndHold = 7,
        CaptureTheFlag = 8,
        Bombmode = 9
    };

    enum status_t
    {
        Updating = 0,
        Up,
        Down
    };

    server_info();

    server_id id;
    status_t status;
    QString name;
    QString gameType; /* q3ut4 */
    player_info_list players;
    int maxPlayerCount;
    QString country;
    game_mode mode;
    QString modeName() const;
    int ping;
    int retries;
    int redScore;
    int blueScore;
    QString map;
    QString mapURL;

    /*! additional server info */
    typedef std::map<QString, QString> Info;
    Info info;

    /*! Info update stamp. Changed after each update. */
    int updateStamp;

private:

};

typedef std::map<server_id, server_info> ServerInfoList;

#endif
