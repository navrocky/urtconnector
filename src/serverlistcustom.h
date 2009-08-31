#ifndef SERVERLISTCUSTOM_H
#define SERVERLISTCUSTOM_H

#include <QObject>
#include "serverinfo.h"
#include "serverid.h"

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

    /*! Resulting server info list */
    const ServerInfoList& list() const {return list_;}

signals:
    void serverChanged(const ServerID& id);
    void serverAdded(const ServerID& id);
    void serverRemoved(const ServerID& id);

protected:
    ServerInfoList list_;

private:
    ServerIDList customServList_;
    bool autoRefresh_;
};



#endif
