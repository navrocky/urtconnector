#include <QStringList>

#include "exception.h"
#include "serverlistqstat.h"
#include "serverid.h"

ServerListQStat::ServerListQStat(QObject *parent)
    : ServerListCustom(parent)
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
//     proc_.
}



