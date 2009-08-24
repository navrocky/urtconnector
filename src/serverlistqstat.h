#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>

#include "serverinfo.h"
#include "serverlistcustom.h"

class ServerListQStat : public ServerListCustom
{
public:
    ServerListQStat();
    ~ServerListQStat();


    const ServerInfoList& list() const {return list_;}

private:
    QProcess proc_;
    ServerInfoList list_;
};

#endif
