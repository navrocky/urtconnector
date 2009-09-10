#ifndef SERVERLISTCUSTOM_H
#define SERVERLISTCUSTOM_H

#include <QObject>
#include <QMutex>
#include "serverinfo.h"
#include "serverid.h"

class ServerListCustom;

/*! Thread safe access to server info list */
class ServerListAccess
{
public:
    ServerListAccess(ServerListCustom*);
    const ServerInfoList& list() const {return list_;}
private:
    QMutexLocker lock_;
    const ServerInfoList& list_;
};

class ServerListCustom : public QObject
{
Q_OBJECT
public:
    ServerListCustom(QObject *parent = 0);
    virtual ~ServerListCustom();

    virtual void refreshAll() = 0;
    virtual void refreshServer(const ServerID& id) = 0;
    virtual void refreshCancel() = 0;

    bool autoRefresh() const {return autoRefresh_;}
    void setAutoRefresh(bool val);

    /*! Custom server list, if empty then server list retrieved from master server */
    ServerIDList& customServList() {return customServList_;}

    /*! List state. Changed always after list change. */
    int state() const {return state_;}

signals:
    void refreshStopped();

protected:
    ServerInfoList list_;
    int state_;
    QMutex listMutex_;

private:
    friend class ServerListAccess;
    ServerIDList customServList_;
    bool autoRefresh_;
};



#endif
