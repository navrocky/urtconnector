#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>

#include "serverinfo.h"
#include "serverlistcustom.h"

class ServerListQStat : public ServerListCustom
{
Q_OBJECT
public:
    ServerListQStat(QObject *parent = 0);
    ~ServerListQStat();

    void refreshAll();
    void refreshServer(const ServerID& id);
    void refreshCancel();

private slots:
    void error( QProcess::ProcessError error );
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void readyReadStandardOutput ();
private:
    QProcess proc_;
    QString qstatPath_;
    QString masterServer_;
    int maxSim_;
};

#endif
