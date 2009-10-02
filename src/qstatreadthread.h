#ifndef QSTATREADTHREAD_H
#define QSTATREADTHREAD_H

#include <QThread>
#include <QString>
#include <QMutex>
#include <QProcess>

#include "qstatoptions.h"
#include "serverinfo.h"

class QStatReadThread : public QThread
{
Q_OBJECT
public:
    QStatReadThread(QObject * parent = 0);

    void setArgs(const QStringList&);
    void setServerInfoList(ServerInfoList*);
    void setListMutex(QMutex*);

    void setQStatOpts(const QStatOptions&);

protected:
    virtual void run ();

signals:
    void error(const QString& msg);
private:
    QStatOptions opts_;
    QStringList args_;
    ServerInfoList* list_;
    QMutex* listMutex_;
//    QProcess proc;
};

#endif
