#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>

#include "serverinfo.h"
#include "serverlistcustom.h"
#include "qstatreadthread.h"

// qstat emulation, otherwise using real qstat
#define QSTAT_FAKE

// qstat out in XML
#define QSTAT_XML

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
    void threadError( const QString& );
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void readyReadStandardOutput ();
private:

    void processLine(const QString& line);
    void applyInfo();

    QProcess proc_;
    QString qstatPath_;
    QString masterServer_;
    int maxSim_;
    ServerInfo curInfo_;
    bool infoFilled_;
    QStatReadThread rd_th_;
};

#endif
