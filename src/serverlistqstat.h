#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>

#include "serverinfo.h"
#include "serverlistcustom.h"

class ServerListQStat : public ServerListCustom
{
public:
    ServerListQStat(QObject *parent = 0);
    ~ServerListQStat();

    virtual void refreshAll();
    virtual void refreshServer(const ServerID& id);
    virtual void refreshCancel();

private:
    QProcess proc_;
    QString qstatPath_;
    QString masterServer_;
    int maxSim_;
};

#endif
