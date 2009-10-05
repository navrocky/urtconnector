#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <map>

#include "serverid.h"
#include "playerinfo.h"

class ServerInfo
{
public:
    enum GameMode
    {
        FreeForAll = 0,
        TeamDeathMatch = 3,
        TeamSurvivor = 4,
        FollowTheLeader = 5,
        CaptureAndHold = 6,
        CaptureTheFlag = 7,
        Bombmode = 8
    };

    enum Status
    {
        Up,
        Down
    };

    ServerInfo();

    ServerID id;
    Status status;
    QString name;
    QString gameType; /* q3ut4 */
    PlayerInfoList players;
    int maxPlayerCount;
    QString country;
    GameMode mode;
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

typedef std::map<ServerID, ServerInfo> ServerInfoList;

#endif
