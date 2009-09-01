#include <QStringList>
#include <QRegExp>

#include "exception.h"
#include "serverlistqstat.h"
#include "serverid.h"

#include <iostream>

using namespace std;

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

//     sl << "-c" << "cat ../doc/qstat_out.txt | awk '{print $0; system(\"usleep 50000\");}'";
//     proc_.start("/bin/bash", sl);

    qstatPath_ = "/usr/bin/qstat";
    masterServer_ = "master.urbanterror.net";

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
    if (proc_.state() == QProcess::NotRunning) return;
    proc_.terminate();
}

void ServerListQStat::error(QProcess::ProcessError error)
{
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
}

void ServerListQStat::readyReadStandardOutput()
{
    while (proc_.canReadLine())
    {
        QString str = QString(proc_.readLine());
        processLine(str);
    }
}

void ServerListQStat::processLine(const QString & line)
{
    cout << line.trimmed().toLocal8Bit().data() << endl;
    try
    {
        QRegExp ServerRx ("^Q3S\\s+(\\d{1,3}.\\d{1,3}.\\d{1,3}.\\d{1,3}:\\d{1,5})\\s+(\\d+)/(\\d+)\\s+(\\d+/\\d+)\\s+([^\\s]+)\\s+(\\d+)\\s*/\\s*(\\d+)\\s+([^\\s]+)\\s+(.+)");
        if (ServerRx.indexIn(line) != -1)
        {
            applyInfo();

            // fill info
            curInfo_ = ServerInfo();
            curInfo_.id = ServerID(ServerRx.cap(1));
            curInfo_.maxPlayerCount = ServerRx.cap(3).toInt();
            curInfo_.map = ServerRx.cap(5).trimmed();
            curInfo_.ping = ServerRx.cap(6).toInt();
            curInfo_.name = ServerRx.cap(9).trimmed();
            infoFilled_ = true;
            //cout << curInfo_.id.address().toLocal8Bit().data() << curInfo_.map.toLocal8Bit().data() << endl;

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
        emit serverAdded(curInfo_.id);
    } else
    {
        list_[curInfo_.id] = curInfo_;
        emit serverChanged(curInfo_.id);
    }

    infoFilled_ = false;
}



