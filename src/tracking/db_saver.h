#ifndef TRACKING_DB_SAVER_H
#define TRACKING_DB_SAVER_H

#include <QObject>

#include "pointers.h"

namespace tracking
{

class manager;
class task_t;

class db_saver : public QObject
{
    Q_OBJECT
public:
    db_saver(manager* list, const condition_factory_p& conds,
             const action_factory_p& acts, QObject* parent);
    
private slots:
    void task_added(task_t*);
    void task_changed(task_t*);
    void task_removed(const QString& id);
    
private:
    manager* list_;
};

}

#endif
