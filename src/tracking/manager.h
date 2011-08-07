#ifndef TRACKING_MANAGER_H
#define	TRACKING_MANAGER_H

#include <QObject>
#include <QMap>
#include <QList>

#include "pointers.h"

namespace tracking
{

class manager : public QObject
{
    Q_OBJECT
public:
    typedef QList<task_t*> task_list_t;

    manager(QObject* parent);

    void add_task(task_t* t);
    void delete_task(task_t* t);

    const task_list_t& tasks() const {return tasks_;}

    void start();
    
signals:
    void changed();

    void task_added(task_t* t);
    void task_removed(const QString& id);
    void task_changed(task_t* t);

private slots:
    void task_destroyed(QObject*);
    void task_changed_slot();

private:
    typedef QMap<task_t*, QString> task_map_t;
    task_map_t task_map_;
    task_list_t tasks_;
};

}

#endif

