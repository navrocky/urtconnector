#ifndef UPDATE_DISPATCHER_H
#define	UPDATE_DISPATCHER_H

#include <QList>
#include <QObject>
#include <QPointer>
#include <jobs/job.h>
#include <jobs/job_queue.h>
#include "../pointers.h"

class update_task;
class geoip;
class job_t;

class update_dispatcher : public QObject
{
    Q_OBJECT
public:
    update_dispatcher(server_list_p list,
                      const geoip& gi,
                      job_queue* que,
                      QObject* parent);

    void set_game_started(bool val);
    
    void add_task(update_task* task);
    void remove_task(update_task* task);

    virtual bool event(QEvent*);

private slots:
    void task_interval_changed();
    void job_state_changed(job_t::state_t state);
    void task_destroyed(QObject* o);

private:
    struct task_rec
    {
        update_task* task;
        int timer_id;
        job_p job;
    };
    typedef QList<task_rec> task_recs_t;

    int find_task_by_timer_id(int);
    int find_task_by_task_p(update_task*);
    int find_task_by_job(job_t*);

    void update_state();
    void execute_task(update_task*);
    void execute_full_update();
    void remove_task_int(update_task*);
    void update_full_update_params();
    void cancel_all_updatings();
    
    task_recs_t tasks_;
    QPointer<job_t> full_update_job_;
    server_list_p list_;
    const geoip& gi_;
    job_queue* que_;
    int full_update_timer_;
    bool game_started_;
};

#endif	/* UPDATE_DISPATCHER_H */

