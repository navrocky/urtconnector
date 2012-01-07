#include "manager.h"

#include <cassert>
#include "task.h"
#include "condition.h"

namespace tracking
{

manager::manager(QObject* parent)
: QObject(parent)
{
}

void manager::add_task(task_t* t)
{
    assert(!t->id().isEmpty());
    connect(t, SIGNAL(changed()), SLOT(task_changed_slot()));
    connect(t, SIGNAL(destroyed(QObject*)), SLOT(task_destroyed(QObject*)));
    tasks_.append(t);
    task_map_[t] = t->id();
    emit changed();
    emit task_added(t);
}

void manager::start()
{
    foreach (task_t* t, tasks_)
    {
        condition_p cond = t->condition();
        assert(cond);
        if (!cond)
            continue;
        if (cond->is_start_needed())
            cond->start();
    }
}

void manager::delete_task(task_t* t)
{
    delete t;
}

void manager::task_destroyed(QObject* o)
{
    task_t* t = static_cast<task_t*>(o);

    task_map_t::iterator it = task_map_.find(t);
    tasks_.removeAll(t);
    assert(it != task_map_.end());
    QString task_id = it.value();
    task_map_.erase(it);
    emit task_removed(task_id);
    emit changed();
}

void manager::task_changed_slot()
{
    task_t* task = qobject_cast<task_t*>(sender());
    assert(task);
    emit changed();
    emit task_changed(task);
}

}
