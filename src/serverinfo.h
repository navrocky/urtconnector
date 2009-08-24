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

    ServerInfo();

    ServerID id;
    QString name;
    PlayerInfoList players;
    int maxPlayerCount;
    QString country;
    GameMode mode;
    QString modeName() const;
    int ping;
    int redScore;
    int blueScore;
    QString map;
    QString mapURL;
    int updateStamp;
private:

};

typedef std::map<ServerID, ServerInfo> ServerInfoList;

#endif
