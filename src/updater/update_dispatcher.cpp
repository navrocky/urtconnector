#include "update_dispatcher.h"

#include <stdint.h>
#include <QTimerEvent>
#include <boost/foreach.hpp>
#include <geoip/geoip.h>
#include "update_task.h"
#include "../job_update_from_master.h"
#include "../job_update_selected.h"

update_dispatcher::update_dispatcher(server_list_p list,
                                     const geoip& gi,
                                     job_queue* que,
                                     QObject* parent)
: QObject(parent)
, list_(list)
, gi_(gi)
, que_(que)
, full_update_timer_(-1)
, game_started_(false)
{
}

void update_dispatcher::set_game_started(bool val)
{
    if (game_started_ == val)
        return;
    game_started_ = val;

    if (game_started_)
    {
        cancel_all_updatings();
    }
}

bool update_dispatcher::event(QEvent* e)
{
    if (e->type() == QEvent::Timer)
    {
        QTimerEvent* te = static_cast<QTimerEvent*>(e);

        if (te->timerId() == full_update_timer_)
        {
            execute_full_update();
        } else
        {
            int i = find_task_by_timer_id(te->timerId());
            if (i >= 0)
            {
                const task_rec& tr = tasks_[i];
                update_task* task = tr.task;
                execute_task(task);
            }
        }
    }

    return QObject::event(e);
}

int update_dispatcher::find_task_by_timer_id(int id)
{
    for (int i = 0; i < tasks_.size(); ++i)
        if (tasks_[i].timer_id == id)
            return i;
    return -1;
}

int update_dispatcher::find_task_by_task_p(update_task* t)
{
    for (int i = 0; i < tasks_.size(); ++i)
        if (tasks_[i].task == t)
            return i;
    return -1;
}

int update_dispatcher::find_task_by_job(job_t* j)
{
    for (int i = 0; i < tasks_.size(); ++i)
        if (tasks_[i].job.get() == j)
            return i;
    return -1;
}

void update_dispatcher::update_full_update_params()
{
    killTimer(full_update_timer_);
    full_update_timer_ = -1;

    // TODO replace to standard const
    const int c_max = 2000000000;

    int interval = c_max;
    foreach (const task_rec& tr, tasks_)
    {
        if (tr.task->servers().isEmpty())
            interval = qMin(tr.task->interval(), interval);
    }
    if (interval != c_max)
    {
        full_update_timer_ = startTimer(interval);
    }
}

void update_dispatcher::add_task(update_task* task)
{
    connect(task, SIGNAL(interval_changed()), SLOT(task_interval_changed()));
    connect(task, SIGNAL(destroyed(QObject*)), SLOT(task_destroyed(QObject*)));

    bool full_update = task->servers().isEmpty();

    task_rec tr;
    tr.task = task;
    if (full_update)
        tr.timer_id = -1;
    else
        tr.timer_id = startTimer(task->interval());
    tasks_.append(tr);

    if (full_update)
    {
        update_full_update_params();
        execute_full_update();
    } else
        execute_task(task);
}

void update_dispatcher::remove_task(update_task* task)
{
    disconnect(task, SIGNAL(interval_changed()), this, SLOT(task_interval_changed()));
    disconnect(task, SIGNAL(destroyed(QObject*)), this, SLOT(task_destroyed(QObject*)));
    remove_task_int(task);
}

void update_dispatcher::task_interval_changed()
{
    update_task* t = qobject_cast<update_task*>(sender());
    if (t->servers().isEmpty())
    {
        update_full_update_params();
        execute_full_update();
    } else
    {
        int i = find_task_by_task_p(t);
        if (i < 0)
            return;
        task_rec& tr = tasks_[i];

        killTimer(tr.timer_id);
        tr.timer_id = startTimer(t->interval());
    }
}

void update_dispatcher::execute_full_update()
{
    if (full_update_job_ || game_started_)
        return;
    job_p j(new job_update_from_master(list_, gi_));
    full_update_job_ = j.get();
    que_->add_job(j);
    connect(j.get(), SIGNAL(state_changed(job_t::state_t)), SLOT(job_state_changed(job_t::state_t)));
}

void update_dispatcher::execute_task(update_task* t)
{
    if (game_started_)
        return;
    int i = find_task_by_task_p(t);
    if (i < 0)
        return;
    task_rec& tr = tasks_[i];

    // is previous job still executed
    if (tr.job)
        return;

    job_p j(new job_update_selected(t->servers(), list_, gi_));
    que_->add_job(j);
    tr.job = j;
    connect(j.get(), SIGNAL(state_changed(job_t::state_t)), SLOT(job_state_changed(job_t::state_t)));
    if (t->single_shot())
    {
        t->set_finished(true);
        remove_task(t);
    }
}

void update_dispatcher::job_state_changed(job_t::state_t state)
{
    job_t* j = qobject_cast<job_t*>(sender());
    assert(j);
    if (!j || !j->is_stopped())
        return;

    if (j == full_update_job_)
    {
        // finish and remove all full update tasks
        task_recs_t tasks_copy = tasks_;
        foreach (const task_rec& tr, tasks_copy)
        {
            if (tr.task && tr.task->single_shot())
            {
                tr.task->set_finished(true);
                remove_task(tr.task);
            }
        }
    } else
    {
        int i = find_task_by_job(j);
        assert(i >= 0);
        if (i < 0)
            return;
        task_rec& tr = tasks_[i];
        if (tr.job)
            disconnect(tr.job.get(), SIGNAL(state_changed(job_t::state_t)), this, SLOT(job_state_changed(job_t::state_t)));
        tr.job.reset();
        if (tr.task->single_shot())
        {
            tr.task->set_finished(true);
            remove_task(tr.task);
        }
    }
}

void update_dispatcher::task_destroyed(QObject* o)
{
    update_task* task = static_cast<update_task*>(o);
    remove_task_int(task);
}

void update_dispatcher::remove_task_int(update_task* task)
{
    int i = find_task_by_task_p(task);
    if (i < 0)
        return;
    task_rec tr = tasks_[i];
    if (tr.timer_id >= 0)
        killTimer(tr.timer_id);
    if (tr.job)
        disconnect(tr.job.get(), SIGNAL(state_changed(job_t::state_t)), this, SLOT(job_state_changed(job_t::state_t)));
    tasks_.removeAt(i);

    if (tr.timer_id < 0)
        update_full_update_params();
}

void update_dispatcher::cancel_all_updatings()
{
    if (full_update_job_)
        full_update_job_->cancel();
    foreach (const task_rec& tr, tasks_)
    {
        if (tr.job)
            tr.job->cancel();
    }
}

