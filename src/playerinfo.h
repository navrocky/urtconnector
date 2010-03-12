#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include <vector>
#include <QString>

class player_info
{
public:
    QString nick_name;
    int score;
    int ping;
};

typedef std::vector<player_info> player_info_list;

#endif
