#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <vector>
#include <QString>

class PlayerInfo
{
public:
    QString nickName;
    int score;
    int ping;
};

typedef std::vector<PlayerInfo> PlayerInfoList;

#endif
