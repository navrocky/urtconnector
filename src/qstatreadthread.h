#ifndef QSTATREADTHREAD_H
#define QSTATREADTHREAD_H

#include <QThread>
#include <QString>
#include <QMutex>
#include <QProcess>

#include "serverinfo.h"

class QStatReadThread : public QThread
{
Q_OBJECT
public:
    QStatReadThread(QObject * parent = 0);

    void setQStatPath(const QString&);
    void setArgs(const QStringList&);
    void setServerInfoList(ServerInfoList*);
    void setListMutex(QMutex*);

protected:
    virtual void run ();

signals:
    void error(const QString& msg);
private:
    const QString& qstatPath_;
    const QStringList& args_;
    ServerInfoList* list_;
    QMutex* listMutex_;
    QProcess proc_;
};

#endif
