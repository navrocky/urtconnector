#include <QThread>
#include <QXmlStreamReader>

#include "qstatreadthread.h"
#include "exception.h"

#include <iostream>

using namespace std;


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
        QProcess proc;
        proc.setReadChannel(QProcess::StandardOutput);
        proc.start(opts_.qstatPath, args_);

        QXmlStreamReader rd;
        while (proc.state() != QProcess::NotRunning)
        {
            proc.waitForReadyRead(1000);
            rd.addData(proc.readAll());

            while (!rd.atEnd())
            {
                if (rd.readNext() != QXmlStreamReader::Invalid)
                {

                    /// здесь обрабатываем очередной элемент XML
                    if (rd.isStartElement())
                        cout << rd.name().toString().toStdString() << endl;
                } else

                if (rd.hasError() && (rd.error() != QXmlStreamReader::PrematureEndOfDocumentError))
                    throw Exception(rd.errorString());
            }

        }

//        proc.kill();
    }
    catch(const Exception& e)
    {
//        proc.kill();
        emit error(QString::fromLocal8Bit(e.what()));
    }
    catch(...)
    {
//        proc.kill();
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

void QStatReadThread::setQStatOpts(const QStatOptions& val)
{
    opts_ = val;
}


