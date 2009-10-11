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
        None = 0,
        FreeForAll = 1,
        TeamDeathMatch = 4,
        TeamSurvivor = 5,
        FollowTheLeader = 6,
        CaptureAndHold = 7,
        CaptureTheFlag = 8,
        Bombmode = 9
    };

    enum Status
    {
        Updating = 0,
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
