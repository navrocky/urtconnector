#ifndef SERVERLISTQSTAT_H
#define SERVERLISTQSTAT_H

#include "serverinfo.h"
#include "serverlistcustom.h"
#include "qstatreadthread.h"
#include "qstatoptions.h"

class ServerListQStat : public ServerListCustom
{
Q_OBJECT
public:
    ServerListQStat(QObject *parent = 0);
    ~ServerListQStat();

    void setQStatOpts(QStatOptions*);

    void refreshAll();
    void refreshServer(const ServerID& id);
    void refreshCancel();

private slots:
    void threadError( const QString& );
    void finished();
private:
    QStatOptions* opts_;
    QStatReadThread rd_th_;
};

#endif
