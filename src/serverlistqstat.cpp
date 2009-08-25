#include <QStringList>

#include "serverlistqstat.h"
#include "serverid.h"

ServerListQStat::ServerListQStat(QObject *parent)
    : ServerListCustom(parent)
{
}


ServerListQStat::~ServerListQStat()
{
}

void ServerListQStat::refreshAll()
{
    if (proc_.state() != QProcess::NotRunning) return;

    QStringList sl;
    //-P -R -pa -ts -nh -q3m master.urbanterror.net
    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh";

    if (customServList().empty())
    {
        sl << "-q3m" << masterServer_;
    } else
    {

    }

    proc_.start(qstatPath_, sl);
}

void ServerListQStat::refreshServer(const ServerID & id)
{
}

void ServerListQStat::refreshCancel()
{
}


