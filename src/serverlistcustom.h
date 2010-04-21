#ifndef SERVERLISTCUSTOM_H
#define SERVERLISTCUSTOM_H

#include <QObject>
#include "serverinfo.h"
#include "serverid.h"
#include "serveroptions.h"

class serv_list_custom : public QObject
{
Q_OBJECT
public:
    serv_list_custom(QObject *parent = 0);
    virtual ~serv_list_custom();

    // servers options
    void setOpts(server_fav_list* opts);

    // update list from internal data without checking online status
    virtual void update() = 0;

    // refresh online status servers from list
    virtual void refreshAll() = 0;
    virtual void refreshServer(const server_id& id) = 0;
    virtual void refreshCancel() = 0;

    bool autoRefresh() const {return autoRefresh_;}
    void setAutoRefresh(bool val);

    /*! Custom server list, if empty then server list retrieved from master server */
    server_id_list& customServList() {return customServList_;}

    /*! Resulting server info list */
    server_info_list_t& list() {return list_;}

    /*! List state. Changed always after list change. */
    int state() const {return state_;}

    void change_state();

signals:
    void refreshStopped();

protected:
    server_info_list_t list_;
    int state_;
    server_fav_list* opts_;

private:
    server_id_list customServList_;
    bool autoRefresh_;
};



#endif
