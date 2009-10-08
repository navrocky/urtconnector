#include <QStringList>
#include <QRegExp>
#include <QByteArray>

#include "exception.h"
#include "serverlistqstat.h"
#include "serverid.h"

#include <iostream>

using namespace std;

const char* c_qstat = "qstat";
const char* c_server = "server";
const char* c_server_type = "type";
const char* c_server_status = "status";
const char* c_hostname = "hostname";
const char* c_name = "name";
const char* c_gametype = "gametype";
const char* c_map = "map";
const char* c_numplayers = "numplayers";
const char* c_maxplayers = "maxplayers";
const char* c_ping = "ping";
const char* c_retries = "retries";
const char* c_rules = "rules";
const char* c_rule = "rule";
const char* c_rule_name = "name";
const char* c_players = "players";
const char* c_player = "player";
const char* c_player_name = "name";
const char* c_player_score = "score";
const char* c_player_ping = "ping";

class XmlParseError: public Exception
{
public:
    XmlParseError(): Exception("Xml parse error") {}
};

ServerListQStat::ServerListQStat(QObject *parent)
    : ServerListCustom(parent),
      maxSim_(10),
      curState_(Init)
{
    connect(&proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
    connect(&proc_, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int,QProcess::ExitStatus)));
    connect(&proc_, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));
}


ServerListQStat::~ServerListQStat()
{
}

void ServerListQStat::refreshAll()
{
    if (proc_.state() != QProcess::NotRunning) return;

    QStringList sl;

    rd_.clear();

#ifdef QSTAT_FAKE
    //sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0;}'";
    proc_.start("/bin/bash", sl);
#else
    qstatPath_ = "/usr/bin/qstat";
    masterServer_ = "master.urbanterror.net";

    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml";
    if (customServList().empty())
    {
        sl << "-q3m" << masterServer_;
    } else
    {
        ServerIDList& list = customServList();
        for (ServerIDList::iterator it = list.begin(); it != list.end(); it++)
            sl << (*it).address();
    }

    proc_.start(qstatPath_, sl);
#endif
}

void ServerListQStat::refreshServer(const ServerID & id)
{
}

void ServerListQStat::refreshCancel()
{
    if (proc_.state() == QProcess::NotRunning) return;
    proc_.terminate();
}

void ServerListQStat::error(QProcess::ProcessError error)
{
    emit refreshStopped();
    switch (error)
    {
        case QProcess::FailedToStart:
            throw Exception(tr("QStat failed to start"));
        case QProcess::Crashed:
            throw Exception(tr("QStat crashed"));
        case QProcess::Timedout:
            throw Exception(tr("QStat timed out"));
        case QProcess::ReadError:
            throw Exception(tr("QStat read error"));
        case QProcess::WriteError:
            throw Exception(tr("QStat write error"));
        case QProcess::UnknownError:
            throw Exception(tr("QStat unknown error"));
    }
    throw Exception(tr("QStat unknown error"));
}

void ServerListQStat::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    emit refreshStopped();
}

void ServerListQStat::readyReadStandardOutput()
{
    rd_.addData(proc_.readAll());

    while (!rd_.atEnd())
    {
        if (rd_.readNext() != QXmlStreamReader::Invalid)
            processXml();
        else

        if (rd_.hasError() && (rd_.error() != QXmlStreamReader::PrematureEndOfDocumentError))
            throw Exception(rd_.errorString());
    }
}

void ServerListQStat::processXml()
{
    if (rd_.isStartElement())
    {
        if (curState_ == Init && rd_.name() == c_qstat)
            curState_ = QStat;

        else if (curState_ == QStat && rd_.name() == c_server)
        {
            if (rd_.attributes().value(c_server_type) == "Q3M")
            {
                curState_ = MasterServer;
            } else
            {
                curServerInfo_ = ServerInfo();
                if (rd_.attributes().value(c_server_status) == "UP")
                    curServerInfo_.status = ServerInfo::Up;
                else
                    curServerInfo_.status = ServerInfo::Down;

                curState_ = Server;
            }
        }
        else if (curState_ == Server && rd_.name() == c_hostname)
            curState_ = HostName;

        else if (curState_ == Server && rd_.name() == c_name)
            curState_ = Name;

        else if (curState_ == Server && rd_.name() == c_gametype)
            curState_ = GameType;

        else if (curState_ == Server && rd_.name() == c_map)
            curState_ = Map;

        else if (curState_ == Server && rd_.name() == c_numplayers)
            curState_ = NumPlayers;

        else if (curState_ == Server && rd_.name() == c_maxplayers)
            curState_ = MaxPlayers;

        else if (curState_ == Server && rd_.name() == c_ping)
            curState_ = Ping;

        else if (curState_ == Server && rd_.name() == c_retries)
            curState_ = Retries;

        else if (curState_ == Server && rd_.name() == c_rules)
            curState_ = Rules;

        else if (curState_ == Rules && rd_.name() == c_rule)
        {
            curRule_ = RuleInfo();
            curRule_.first = rd_.attributes().value(c_rule_name).toString();
            curState_ = Rule;
        }

        else if (curState_ == Server && rd_.name() == c_players)
            curState_ = Players;

        else if (curState_ == Players && rd_.name() == c_player)
        {
            curPlayerInfo_ = PlayerInfo();
            curState_ = Player;
        }

        else if (curState_ == Player && rd_.name() == c_player_name)
            curState_ = PlayerName;
        else if (curState_ == Player && rd_.name() == c_player_ping)
            curState_ = PlayerPing;
        else if (curState_ == Player && rd_.name() == c_player_score)
            curState_ = PlayerScore;

    } else

    if (rd_.isCharacters())
    {
        if (curState_ == HostName)
            curServerInfo_.id = ServerID(rd_.text().toString());
        else if (curState_ == Name)
            curServerInfo_.name = rd_.text().toString();
        else if (curState_ == GameType)
            curServerInfo_.gameType = rd_.text().toString();
        else if (curState_ == Map)
            curServerInfo_.map = rd_.text().toString();
        else if (curState_ == MaxPlayers)
            curServerInfo_.maxPlayerCount = rd_.text().toString().toInt();
        else if (curState_ == Ping)
            curServerInfo_.ping = rd_.text().toString().toInt();
        else if (curState_ == Retries)
            curServerInfo_.retries = rd_.text().toString().toInt();
        else if (curState_ == Rule)
            curRule_.second = rd_.text().toString().toInt();
        else if (curState_ == PlayerName)
            curPlayerInfo_.nickName = rd_.text().toString();
        else if (curState_ == PlayerScore)
            curPlayerInfo_.score = rd_.text().toString().toInt();
        else if (curState_ == PlayerPing)
            curPlayerInfo_.ping = rd_.text().toString().toInt();
    } else

    if (rd_.isEndElement())
    {
        if (rd_.name() == c_server)
        {
            if (curState_ == Server)
            {
                list_[curServerInfo_.id] = curServerInfo_;
                state_++;
            }
            curState_ = QStat;
        }
        else if (rd_.name() == c_qstat)
            curState_ = Init;
        else if (rd_.name() == c_hostname || rd_.name() == c_name || rd_.name() == c_gametype
          || rd_.name() == c_map || rd_.name() == c_numplayers || rd_.name() == c_maxplayers
          || rd_.name() == c_ping || rd_.name() == c_retries || rd_.name() == c_rules || rd_.name() == c_players )
            curState_ = Server;
        else if (rd_.name() == c_rule)
        {
            curServerInfo_.info[curRule_.first] = curRule_.second;
            curState_ = Rules;
        }
        else if (rd_.name() == c_player)
        {
            curServerInfo_.players.push_back(curPlayerInfo_);
            curState_ = Players;
        }
        else if (rd_.name() == c_player_name || rd_.name() == c_player_ping || rd_.name() == c_player_score)
            curState_ = Player;
    }
//     throw XmlParseError();

}

void ServerListQStat::update()
{
    ServerIDList& list = customServList();
    for (ServerIDList::iterator it = list.begin(); it != list.end(); it++)
    {
        ServerID id = *it;
        ServerInfo info = list_[id];

        info.id = id;

        ServerOptions* opt = find_options_by_id(opts_, id);
        if (opt)
        {
            info.name = opt->name();
        }
        list_[id] = info;
    }
    state_++;
}




