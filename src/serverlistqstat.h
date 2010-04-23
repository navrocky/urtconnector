#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include <QProcess>
#include <QXmlStreamReader>

#include "server_info.h"
#include "serverlistcustom.h"
#include "qstat_options.h"

// qstat emulation, otherwise using real qstat
//#define QSTAT_FAKE

class ServerListQStat : public serv_list_custom
{
Q_OBJECT
public:
    ServerListQStat(QObject *parent = 0);
    ~ServerListQStat();

    void update();

    void setQStatOpts(qstat_options* opts);

    void refreshAll();
    void refreshServer(const server_id& id);
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
    server_info curServerInfo_;
    player_info curPlayerInfo_;
    qstat_options* qstatOpts_;
    typedef std::pair<QString, QString> RuleInfo;
    RuleInfo curRule_;
};

#endif
