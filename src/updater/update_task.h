#ifndef UPDATE_TASK_H
#define	UPDATE_TASK_H

#include <QObject>
#include <common/server_id.h>

class update_task : public QObject
{
    Q_OBJECT
public:
    update_task(QObject* parent);

    const server_id_list& servers() const {return servers_;}
    void set_servers(const server_id_list&);

    bool single_shot() const {return single_shot_;}
    void set_single_shot(bool);

    // used if single shoot
    bool is_finished() const {return is_finished_;}
    void set_finished(bool val) {is_finished_ = val;}

    int interval() const {return interval_;}
    void set_interval(int);
    
signals:
    void interval_changed();

private:
    bool single_shot_;
    int interval_;
    server_id_list servers_;
    bool is_finished_;
};

#endif	/* UPDATE_TASK_H */

