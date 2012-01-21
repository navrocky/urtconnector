
#include <iostream>

#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include <backends/gdocs/gdocs.h>
#include "manager.h"

#include "settings/settings.h"
#include "settings.h"

#include "storage.h"

#include "cl/syslog/syslog.h"
#include "common/qt_syslog.h"

using namespace std;
using namespace boost;

SYSLOG_MODULE(syncro_manager);

#define SYNC_DEBUG LOG_DEBUG << " >> " << __PRETTY_FUNCTION__ << ": "

namespace remote {


    
    
template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
    return *const_cast<T*>(ptr.get());
}

struct gdocs_service: public service {

    gdocs_service()
        : service("gdocs", "gdocs service")
    {}
    
    service::Storage do_create(boost::shared_ptr<QSettings> settings) const {
        std::cerr<<"do_create"<<std::endl;
        service::Storage s = service::Storage(new gdocs( "kinnalru@gmail.com", "malder11", "urt" ));
        std::cerr<<"do_create ok"<<std::endl;
        return s;
    }
};


syncro_manager::syncro_manager()
{
    services_.push_back( Service(new gdocs_service) );
}

const std::list<syncro_manager::Service>& syncro_manager::services() const
{
    return services_;   
}

std::list<syncro_manager::Object> syncro_manager::objects() const
{
    return std::list<syncro_manager::Object>(
        boost::make_transform_iterator(objects_.begin(), boost::bind(&Objects::value_type::first, _1)),
        boost::make_transform_iterator(objects_.end(), boost::bind(&Objects::value_type::first, _1))
    );
}

list<syncro_manager::Storage> syncro_manager::storages() const
{
    return storages_;
}


syncro_manager::Storage syncro_manager::create(const Service& srv)
{
    std::list<Service>::iterator it = boost::find(services_, srv);
    
    if (it == services_.end())
        throw std::logic_error("so such service registered");
    
    remote::service& service = cast(srv);
    
    base_settings main;
    base_settings::qsettings_p self_options = main.get_settings(manager_options::uid());

    const QString options_dir = QFileInfo(self_options->fileName()).dir().path();
    const QString services_path = options_dir + "/services/" + service.caption();
    const QString storage_path = services_path + "/plugin1";
   
    
    manager_options mo;
    
    mo.storages_set( mo.storages() << storage_path );

    main.register_file("gdocs_uuid", storage_path, false);
    
    Storage storage = *storages_.insert(storages_.end(), service.create(main.get_settings("gdocs_uuid")));

    SYNC_DEBUG << "storage created";

    return storage;
}

void syncro_manager::bind(const Object& obj, const Storage& storage)
{
    objects_[obj].insert(storage);
}


void syncro_manager::sync(const Object& obj)
{
    SYNC_DEBUG << "s1";
    if (boost::find_if(tasks_, boost::bind(&sync_task::object, _1) == obj) != tasks_.end())
    {
        SYNC_DEBUG << "s2";
        return;        
    }
 
    SYNC_DEBUG << "s3";
    sync_task& task = *tasks_.insert(tasks_.end(), sync_task(obj, objects_[obj], obj->get()));
    SYNC_DEBUG << "s4";
    storage& storage = cast(task.current_storage());
    
    SYNC_DEBUG << "s5";
    task.action = storage.get(task.group.type());
    Q_ASSERT(connect(task.action, SIGNAL(loaded(const remote::group&)), SLOT(loaded(const remote::group&))));
    assert(connect(task.action, SIGNAL(error(const QString&)), SLOT(error(const QString&))));
    assert(connect(task.action, SIGNAL(finished()), SLOT(finished())));
    
    SYNC_DEBUG << "s6";
    task.action->start();    
    SYNC_DEBUG << "s7";
}

void syncro_manager::put(const remote::syncro_manager::Object& obj)
{
    SYNC_DEBUG << "p1";
    if (boost::find_if(tasks_, boost::bind(&sync_task::object, _1) == obj) != tasks_.end())
    {
        SYNC_DEBUG << "p2";
        return;        
    }
 
    SYNC_DEBUG << "p3";
    sync_task& task = *tasks_.insert(tasks_.end(), sync_task(obj, objects_[obj], obj->get()));
    SYNC_DEBUG << "p4";
    storage& storage = cast(task.current_storage());
    
    SYNC_DEBUG << "p5";
    task.action = storage.put(obj->get());
    Q_ASSERT(connect(task.action, SIGNAL(saved()), SLOT(saved())));
    assert(connect(task.action, SIGNAL(error(const QString&)), SLOT(error(const QString&))));
    assert(connect(task.action, SIGNAL(finished()), SLOT(finished())));
    
    SYNC_DEBUG << "s6";
    task.action->start();    
    SYNC_DEBUG << "s7";
}


void syncro_manager::loaded(const remote::group& obj)
{
    SYNC_DEBUG << "l1--------------------";
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);
    
    task.entries = remote::merge(obj.entries(), task.entries);
    SYNC_DEBUG << "l2";
}

void syncro_manager::saved()
{
    SYNC_DEBUG << "save1";
    
    SYNC_DEBUG << "save2";
}


void syncro_manager::error(const QString& err)
{
    SYNC_DEBUG << "e1:" << err;
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);
    SYNC_DEBUG << "e2";
}

void syncro_manager::finished()
{
    SYNC_DEBUG << "f1";
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);    
    
    SYNC_DEBUG << "f2";
    task.storages.erase(task.current_storage());

    if (task.storages.empty())
    {
        SYNC_DEBUG << "f3";
        cast(task.object).put(group(task.group.type(), task.entries));
        SYNC_DEBUG << "f4";
        tasks_.remove(task);
        SYNC_DEBUG << "f5";
    }
    SYNC_DEBUG << "f6";
}

struct equal_name {
    const QString& name;
    equal_name(const QString& name): name(name) {}
    
    template <typename Pair>
    bool operator()(const Pair& pair)
    { return pair.first->name() == name; }
};

syncro_manager::Object syncro_manager::attach(const QString& name, const Getter& g, const Setter& s, const QString& desc)
{
    if (boost::find_if(objects_, equal_name(name)) != objects_.end())
        throw std::runtime_error("Object with such name already attached!");

    Object obj(new object(g, s, name, desc));
    objects_.insert(std::make_pair(obj, Storages()));
    
    return obj;
}

void syncro_manager::detach(const syncro_manager::Object& obj)
{
    if (objects_.erase(obj) == 0)
        throw std::runtime_error("No object with such name attached!");
}


};