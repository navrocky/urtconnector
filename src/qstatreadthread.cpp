#include <QThread>
#include <QXmlStreamReader>

#include "qstatreadthread.h"
#include "exception.h"

QStatReadThread::QStatReadThread(QObject * parent) :
    QThread(parent),
    list_(0),
    listMutex_(0)
{
}

void QStatReadThread::run ()
{
    try
    {
        proc_.start(opts_.qstatPath, args_);

        QXmlStreamReader rd;
        rd.setDevice(&proc_);

        while (!rd.atEnd())
        {
            rd.readNext();
            if (rd.isStartElement() && (rd.name() == "qstat"))
            {
                //parseQStat();
            } else
                throw Exception(tr("Error in qstat xml output"));
        }
        if (rd.hasError())
            throw Exception(rd.errorString());

        proc_.kill();
    }
    catch(const Exception& e)
    {
        proc_.kill();
        emit error(QString::fromLocal8Bit(e.what()));
    }
    catch(...)
    {
        proc_.kill();
        emit error(tr("Unknown error"));
    }
}

void QStatReadThread::setArgs(const QStringList &val)
{
    args_ = val;
}

void QStatReadThread::setServerInfoList(ServerInfoList *val)
{
    list_ = val;
}

void QStatReadThread::setListMutex(QMutex *val)
{
    listMutex_ = val;
}

void QStatReadThread::setQStatOpts(const QStatOptions&)
{
}


