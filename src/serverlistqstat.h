#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>
#include <QXmlStreamReader>

#include "serverinfo.h"
#include "serverlistcustom.h"

// qstat emulation, otherwise using real qstat
//#define QSTAT_FAKE

// qstat out in XML
//#define QSTAT_XML

class ServerListQStat : public ServerListCustom
{
Q_OBJECT
public:
    ServerListQStat(QObject *parent = 0);
    ~ServerListQStat();

    void update();

    void refreshAll();
    void refreshServer(const ServerID& id);
    void refreshCancel();

private slots:
    void error( QProcess::ProcessError error );
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void readyReadStandardOutput ();
private:

    void processXml();

    QProcess proc_;
    QString qstatPath_;
    QString masterServer_;
    int maxSim_;

    QXmlStreamReader rd_;

    enum State
    {
        Init, QStat, MasterServer, Server, HostName, Name, GameType, Map, NumPlayers, MaxPlayers, Ping,
        Retries, Rules, Rule, Players, Player, PlayerName, PlayerScore, PlayerPing
    };
    State curState_;
    ServerInfo curServerInfo_;
    PlayerInfo curPlayerInfo_;
    typedef std::pair<QString, QString> RuleInfo;
    RuleInfo curRule_;
};

#endif
