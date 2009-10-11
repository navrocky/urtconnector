#include <QObject>

#include "serverinfo.h"

ServerInfo::ServerInfo()
  : status(Up),
    maxPlayerCount(0),
    mode(FreeForAll),
    ping(0),
    retries(0),
    redScore(0),
    blueScore(0),
    updateStamp(0)
{
}

QString ServerInfo::modeName() const
{
    switch (mode)
    {
        case None: return QString();
        case FreeForAll: return QObject::tr("Free For All");
        case TeamDeathMatch: return QObject::tr("Team DeathMatch");
        case TeamSurvivor: return QObject::tr("Team Survivor");
        case FollowTheLeader: return QObject::tr("Follow The Leader");
        case CaptureAndHold: return QObject::tr("Capture And Hold");
        case CaptureTheFlag: return QObject::tr("Capture The Flag");
        case Bombmode: return QObject::tr("Bomb Mode");
    }
    return QObject::tr("Unknown");
}




