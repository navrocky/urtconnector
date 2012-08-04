
#include <boost/bind.hpp>

#include "cl/syslog/syslog.h"
#include "common/qt_syslog.h"

#include "task.h"

SYSLOG_MODULE(syncro_manager::task);

using namespace remote;

///const cast pointer to refference(to convert "key" objects to "useful" objects)
template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
    return *const_cast<T*>(ptr.get());
}

struct syncro_manager::get_task : public syncro_manager::task {
    Object object;
    Storages storages;
    remote::group group;
    remote::group::Entries entries;
    Status st;
    
    remote::action* action;

    get_task(const Object& o, const Storages& st, const remote::group& gr)
        : object(o), storages(st), group(gr), action(0)
        , entries(group.entries()), st(None)
    {}

    virtual void start() {
        start_get();
    }
    
    void start_get() {
        Q_ASSERT(!storages.empty());

        action = cast(*storages.begin()).get(group.type());

        connect(action, SIGNAL(loaded(const remote::group&)), SLOT(loaded_one(const remote::group&)));
        connect(action, SIGNAL(error(QString)), SLOT(error_one(QString)));
        connect(action, SIGNAL(finished()), SLOT(finished_one()));
        action->start();
    }

    virtual void error_one(const QString& err) {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check        
        
        emit error(err);
    }
    
    virtual void loaded_one(const remote::group& obj) {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

        try {
            entries = remote::merge(entries, entries);        
        } 
        catch(...)
        {}
    }
    
    virtual void finished_one() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
        Q_ASSERT(!storages.empty());

        storages.erase(storages.begin());

        if (storages.empty())
        {
            st |= Finished;
            emit finished();
            emit completed(object, remote::group(group.type(), entries));
        }
        else
        {
            start_get();
        }
    }
    
    virtual void abort() {
        action->abort();
        storages.clear();
        
        st |= Aborted;        
        emit finished();
    };
    
    virtual Status status() const {
        return st;
    }
};


struct syncro_manager::put_task : public syncro_manager::task {
    Object object;
    Storages storages;
    remote::group group;
    remote::group::Entries entries;
    Status st;
    
    remote::action* action;

    put_task(const Object& o, const Storages& st, const remote::group& gr)
        : object(o), storages(st), group(gr), action(0)
        , entries(group.entries()), st(None)
    {}

    virtual void start() {
        start_put();
    }

    void start_put() {
        Q_ASSERT(!storages.empty());

        action = cast(*storages.begin()).put(group);

        connect(action, SIGNAL(saved()), SLOT(saved_one()));
        connect(action, SIGNAL(error(QString)), SLOT(error_one(QString)));
        connect(action, SIGNAL(finished()), SLOT(finished_one()));
        action->start();
    }

    virtual void error_one(const QString& err) {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check        
        
        emit error(err);
    }

    virtual void saved_one() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
    }

    virtual void finished_one() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
        Q_ASSERT(!storages.empty());

        storages.erase(storages.begin());
        
        if (storages.empty())
        {
            st |= Finished;
            emit completed(object, group);
            emit finished();            
        }
        else
        {
            start_put();
        }
    }
    
    virtual void abort() {
        action->abort();
        storages.clear();
        
        st |= Aborted;        
        emit finished();
    };
    
    virtual Status status() const {
        return st;
    }
};

struct syncro_manager::sync_task : public syncro_manager::task {
    get_task* gt;
    put_task* pt;
    Status st;
    
    sync_task(const Object& o, const Storages& st, const remote::group& gr)
        : gt(new get_task(o, st, gr))
        , pt(0)
    {
        connect(gt, SIGNAL(error(QString)), SLOT(error_one(QString)));
        connect(gt, SIGNAL(finished()), SLOT(finished_one()));
    }
    
    virtual void start() {
        gt->start();
    }
    
    virtual void error_one(const QString& err) {
        emit error(err);
    }
    
    virtual void finished_one() {
        if(sender() == gt)
        {
            if (!(gt->status() & Aborted))
            {
                Q_ASSERT(gt->status() == Finished);
                
                remote::group::Entries entries = gt->entries;
                remote::group::Entries non_deleted;

                std::remove_copy_if(entries.begin(), entries.end(), std::inserter(non_deleted, non_deleted.end()),
                    boost::bind(&remote::intermediate::is_deleted, _1));

                entries.swap(non_deleted);
                
                pt = new put_task(gt->object, gt->storages, remote::group(gt->group.type(), entries));
                gt->deleteLater();
                gt = 0;
                pt->start();
            }
        }
        else if(sender() == pt)
        {
            if (!(pt->status() & Aborted))
            {
                Q_ASSERT(pt->status() == Finished);
                st |= Finished;
                emit completed(pt->object, remote::group(pt->group.type(), pt->entries));
                pt->deleteLater();
                pt = 0;
                emit finished();
            }
        }
        else
        {
            Q_ASSERT(!"something goes very wrong in sync_task logic");
        }
    }
    
    virtual void abort() {
        if (gt) gt->abort();
        
        if (pt) pt->abort();
        
        st |= Aborted;        
        emit finished();
    }
    
    virtual Status status() const {
        return st;
    }
};
/*
struct syncro_manager::sync_task : public syncro_manager::task {
    Object object;
    Storages get_storages;
    Storages put_storages;
    remote::group group;
    remote::action* action;
    remote::group::Entries entries;
    Status st;

    sync_task(const Object& o, const Storages& st, const remote::group& gr)
        : object(o), get_storages(st), put_storages(st), group(gr), action(0)
        , entries(group.entries()), st(None)
    {}

    virtual void start() {
        LOG_INFO << "start...";
        start_get();
    }

    void start_get() {
        Q_ASSERT(!get_storages.empty());

        LOG_INFO << " <<< start get...";
        action = cast(*get_storages.begin()).get(group.type());

        connect(action, SIGNAL(loaded(const remote::group&)), SLOT(loaded(const remote::group&)));
        connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
        connect(action, SIGNAL(finished()), SLOT(finished()));
        action->start();
    }

    void start_put() {
        LOG_INFO << "start PUT... >>> ";
        Q_ASSERT(!put_storages.empty());

        action = cast(*put_storages.begin()).put(group);

        connect(action, SIGNAL(saved()), SLOT(saved()));
        connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
        connect(action, SIGNAL(finished()), SLOT(finished()));
        action->start();
    }

    virtual void loaded(const remote::group& obj) {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

        LOG_INFO << "LAODED";
        entries = remote::merge(obj.entries(), entries);
    }

    virtual void error(const QString& error) {
        LOG_ERR << "Error:" << error;
    }

    virtual void saved() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
        LOG_INFO << "SAVED >>>";
    }

    virtual void finished() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check

        LOG_INFO << "finished...";
        (!get_storages.empty())
            ? get_finished()
            : put_finished();
    }

    void get_finished() {
        Q_ASSERT(!get_storages.empty());

        LOG_INFO << " <<< finihsed get ...";
        get_storages.erase(get_storages.begin());
        if (get_storages.empty())
        {
            LOG_INFO << " <<< finihsed get ALL";
            remote::group::Entries non_deleted;

            std::remove_copy_if(entries.begin(), entries.end(), std::inserter(non_deleted, non_deleted.end()),
                boost::bind(&remote::intermediate::is_deleted, _1));

            entries.swap(non_deleted);
            group = remote::group(group.type(), entries);
            start_put();
        }
        else
        {
            LOG_INFO << " <<< partial";
            start_get();
        }
    }

    void put_finished() {
        Q_ASSERT(get_storages.empty());
        Q_ASSERT(!put_storages.empty());

        LOG_INFO << " <<< finihsed put ...";

        put_storages.erase(put_storages.begin());
        if (put_storages.empty())
        {
            LOG_INFO << " <<< finihsed put ALL";
            st = Finished;
            emit completed(object, group);
        }
        else
        {
            LOG_INFO << " <<< partial put";
            start_put();
        }
    }
    
    virtual void abort() {};
    
    virtual Status status() const
    {
        return st;
    }
};*/

boost::shared_ptr<syncro_manager::task>
syncro_manager::task_factory::create_get(const syncro_manager::Object& o, const syncro_manager::Storages& st, const group& gr)
{
    return boost::shared_ptr<syncro_manager::task>(new syncro_manager::get_task(o, st, gr));
}

boost::shared_ptr<syncro_manager::task>
syncro_manager::task_factory::create_sync(const syncro_manager::Object& o, const syncro_manager::Storages& st, const group& gr)
{
    return boost::shared_ptr<syncro_manager::task>(new syncro_manager::sync_task(o, st, gr));
}

boost::shared_ptr<syncro_manager::task>
syncro_manager::task_factory::create_put(const syncro_manager::Object& o, const syncro_manager::Storages& st, const group& gr)
{
    return boost::shared_ptr<syncro_manager::task>(new syncro_manager::put_task(o, st, gr));
}
