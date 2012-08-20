 
#ifndef URT_REMOTE_TASK_H
#define URT_REMOTE_TASK_H

#include <QObject>

#include "manager.h"

namespace remote {

class syncro_manager::task : public remote::action {
Q_OBJECT

public:
    virtual const Object& object() const = 0;
    
Q_SIGNALS:
    void completed(const syncro_manager::Object&);
    void completed(const syncro_manager::Object&, const remote::group&);

private Q_SLOTS:
    virtual void loaded_one(const remote::group& obj) {};
    virtual void saved_one() {};
    virtual void error_one(const QString& error) {};
    virtual void finished_one() {};
};

struct syncro_manager::task_factory {

    static boost::shared_ptr<syncro_manager::task>
    create_get(const syncro_manager::Object& o, const syncro_manager::Storages& st, const remote::group& gr);

    static boost::shared_ptr<syncro_manager::task>
    create_sync(const syncro_manager::Object& o, const syncro_manager::Storages& st, const remote::group& gr);
    
    static boost::shared_ptr<syncro_manager::task>
    create_put(const syncro_manager::Object& o, const syncro_manager::Storages& st, const remote::group& gr);

};

} //namespace task

#endif


