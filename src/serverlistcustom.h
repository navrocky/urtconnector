#ifndef SERVERLISTCUSTOM_H
#define SERVERLISTCUSTOM_H

#include <QObject>
#include "serverinfo.h"
#include "serverid.h"
#include "serveroptions.h"

class ServerListCustom : public QObject
{
Q_OBJECT
public:
    ServerListCustom(QObject *parent = 0);
    virtual ~ServerListCustom();

    // servers options
    void setOpts(ServerOptionsList* opts);

    // update list from internal data without checking online status
    virtual void update() = 0;

    // refresh online status servers from list
    virtual void refreshAll() = 0;
    virtual void refreshServer(const ServerID& id) = 0;
    virtual void refreshCancel() = 0;

    bool autoRefresh() const {return autoRefresh_;}
    void setAutoRefresh(bool val);

    /*! Custom server list, if empty then server list retrieved from master server */
    ServerIDList& customServList() {return customServList_;}

    /*! Resulting server info list */
    const ServerInfoList& list() const {return list_;}

    /*! List state. Changed always after list change. */
    int state() const {return state_;}

signals:
    void refreshStopped();

protected:
    ServerInfoList list_;
    int state_;
    ServerOptionsList* opts_;

private:
    ServerIDList customServList_;
    bool autoRefresh_;
};



#endif
