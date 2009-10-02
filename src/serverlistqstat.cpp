#include <QStringList>
#include <QRegExp>

#include "exception.h"
#include "serverlistqstat.h"
#include "serverid.h"
#include "qstatoptions.h"

#include <iostream>

using namespace std;

ServerListQStat::ServerListQStat(QObject *parent)
    : ServerListCustom(parent)
{
//    connect(&proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(error(QProcess::ProcessError)));
//    connect(&proc_, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(finished(int,QProcess::ExitStatus)));
//    connect(&proc_, SIGNAL(readyReadStandardOutput()), SLOT(readyReadStandardOutput()));

    connect(&rd_th_, SIGNAL(error(const QString&)), SLOT(threadError(const QString&)));
    connect(&rd_th_, SIGNAL(finished()), SLOT(finished()));

    rd_th_.setServerInfoList(&list_);
    rd_th_.setListMutex(&listMutex_);
}


ServerListQStat::~ServerListQStat()
{
}

void ServerListQStat::refreshAll()
{
    QStringList args;
    args << "-P" << "-R" << "-pa" << "-ts" << "-nh" << "-xml";

    if (customServList().empty())
        args << "-q3m" << opts_->masterServer;
    else
        for (unsigned int i = 0; i < customServList().size(); i++)
            args << customServList()[i].address();

    // FAKE OUT
    args.clear();
    args << "-c" << "cat ../doc/ExampleData/qstat_out.xml | awk '{print $0; system(\"usleep 50000\");}'";
    opts_->qstatPath = "/bin/bash";
    //

    rd_th_.setQStatOpts(*opts_);
    rd_th_.setArgs(args);
    rd_th_.start();
}

void ServerListQStat::finished()
{
    emit refreshStopped();
}

void ServerListQStat::refreshServer(const ServerID &)
{
}

void ServerListQStat::refreshCancel()
{
    rd_th_.terminate();
}

//void ServerListQStat::error(QProcess::ProcessError error)
//{
//    emit refreshStopped();
//    switch (error)
//    {
//        case QProcess::FailedToStart:
//            throw Exception(tr("QStat failed to start"));
//        case QProcess::Crashed:
//            throw Exception(tr("QStat crashed"));
//        case QProcess::Timedout:
//            throw Exception(tr("QStat timed out"));
//        case QProcess::ReadError:
//            throw Exception(tr("QStat read error"));
//        case QProcess::WriteError:
//            throw Exception(tr("QStat write error"));
//        case QProcess::UnknownError:
//            throw Exception(tr("QStat unknown error"));
//    }
//    throw Exception(tr("QStat unknown error"));
//}

void ServerListQStat::threadError(const QString &str)
{
    throw Exception(str);
}

void ServerListQStat::setQStatOpts(QStatOptions* val)
{
    opts_ = val;
}




