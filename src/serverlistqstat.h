#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>
#include <QXmlStreamReader>

#include "serverinfo.h"
#include "serverlistcustom.h"
#include "qstatoptions.h"

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

    void setQStatOpts(QStatOptions* opts);

    void refreshAll();
    void refreshServer(const ServerID& id);
    void refreshCancel();

private slots:
    void error( QProcess::ProcessError error );
    void finished ( int exitCode, QProcess::ExitStatus exitStatus );
    void readyReadStandardOutput ();
private:

    void processXml();
    void prepareInfo();

    QProcess proc_;

    QXmlStreamReader rd_;

    enum State
    {
        Init, QStat, MasterServer, Server, HostName, Name, GameType, Map, NumPlayers, MaxPlayers, Ping,
        Retries, Rules, Rule, Players, Player, PlayerName, PlayerScore, PlayerPing
    };
    State curState_;
    ServerInfo curServerInfo_;
    PlayerInfo curPlayerInfo_;
    QStatOptions* qstatOpts_;
    typedef std::pair<QString, QString> RuleInfo;
    RuleInfo curRule_;
};

#endif
