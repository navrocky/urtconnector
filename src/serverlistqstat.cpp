#include <QStringList>
#include <QRegExp>

#include "exception.h"
#include "serverlistqstat.h"
#include "serverid.h"

#include <iostream>

using namespace std;

class QStatThread : public QThread
{
public:
    QStatThread(const QString& qstatPath, const QStringList& args, ServerInfoList& list, QMutex* listMutex, QObject * parent = 0);
protected:
    virtual void run ();
private:
    const QString& qstatPath_;
    const QStringList& args_;
    ServerInfoList& list_;
    QMutex* listMutex_;
};

QStatThread::QStatThread(const QString& qstatPath, const QStringList& args, ServerInfoList& list, QMutex* listMutex, QObject * parent) :
    QThread(parent),
    qstatPath_(qstatPath),
    args_(args),
    list_(list),
    listMutex_(listMutex)
{
}

void QStatThread::run ()
{
    QProcess proc;
    proc.start(qstatPath_, args_);
    while (proc.state() != QProcess::NotRunning)
    {





    }
}


ServerListQStat::ServerListQStat(QObject *parent)
    : ServerListCustom(parent),
      maxSim_(10),
      infoFilled_(false)
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

#ifdef QSTAT_FAKE
    #ifdef QSTAT_XML
        sl << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    #else
        sl << "-c" << "cat ../doc/qstat_out.txt | awk '{print $0; system(\"usleep 50000\");}'";
    #endif
    proc_.start("/bin/bash", sl);

#else
    qstatPath_ = "/usr/bin/qstat";
    masterServer_ = "master.urbanterror.net";

    sl << "-P" << "-R" << "-pa" << "-ts" << "-nh";
    #ifdef QSTAT_XML
        sl << "-xml"
    #endif
    if (customServList().empty())
    {
        sl << "-q3m" << masterServer_;
    } else
    {

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
    while (proc_.canReadLine())
    {
        QString str = QString(proc_.readLine());
        #ifdef QSTAT_XML
        processLineXml(str);
        #else
        processLine(str);
        #endif
    }
}

void ServerListQStat::processLine(const QString & line)
{
//     cout << line.trimmed().toStdString() << endl;
    try
    {
        QRegExp ServerRx ("^Q3S\\s+(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}:\\d{1,5})\\s+(\\d+)/(\\d+)\\s+(\\d+/\\d+)\\s+([^\\s]+)\\s+(\\d+)\\s*/\\s*(\\d+)\\s+([^\\s]+)\\s+(.+)");
        QRegExp ServInfoRx("^\t([\\w]+=[^,]+,)*([\\w]+=.+)$");
        QRegExp PlayerRx("^\t\\s*(-?\\d+)\\s+frags\\s+(\\d+)ms\\s+(.+)$");
        if (ServerRx.indexIn(line) != -1)
        {
            applyInfo();

            // filter other then q3ut4 games
            if (ServerRx.cap(8) != "q3ut4") return;

            // fill info
            curInfo_ = ServerInfo();
            curInfo_.id = ServerID(ServerRx.cap(1));
            curInfo_.maxPlayerCount = ServerRx.cap(3).toInt();
            curInfo_.map = ServerRx.cap(5).trimmed();
            curInfo_.ping = ServerRx.cap(6).toInt();
            curInfo_.name = ServerRx.cap(9).trimmed();
            infoFilled_ = true;
        } else
        if (ServInfoRx.indexIn(line) != -1)
        {
            if (!infoFilled_) return;
            QStringList sl = line.trimmed().split(',');
            QRegExp rx("^([^=]+)=(.+)$");
            for (QStringList::iterator it = sl.begin(); it != sl.end(); it++)
                if (rx.exactMatch(*it))
                    curInfo_.info[rx.cap(1)] = rx.cap(2);

            curInfo_.mode = static_cast<ServerInfo::GameMode>(curInfo_.info["g_gametype"].toInt());

        } else
        if (PlayerRx.indexIn(line) != -1)
        {
            if (!infoFilled_) return;
            PlayerInfo pi;
            pi.score = PlayerRx.cap(1).toInt();
            pi.ping = PlayerRx.cap(2).toInt();
            pi.nickName = PlayerRx.cap(3).trimmed();
            curInfo_.players.push_back(pi);
        }
    }
    catch(...)
    {}
}

void ServerListQStat::applyInfo()
{
    if (!infoFilled_) return;

    if (list_.find(curInfo_.id) == list_.end())
    {
        list_[curInfo_.id] = curInfo_;
        state_++;
    } else
    {
        const ServerInfo& old = list_[curInfo_.id];
        curInfo_.updateStamp = old.updateStamp + 1;
        list_[curInfo_.id] = curInfo_;
        state_++;
    }

    infoFilled_ = false;
}

void ServerListQStat::processLineXml(const QString & line)
{

}





