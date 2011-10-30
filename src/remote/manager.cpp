
#include <iostream>

#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <QFileInfo>
#include <QDir>
#include <QSettings>

#include <backends/gdocs/gdocs.h>
#include "manager.h"

#include "settings/settings.h"
#include "settings.h"

#include "storage.h"

using namespace std;
using namespace boost;
 
namespace remote {

// const manager::ObjectCompare c_compare = boost::bind(&manager::subject::name, _1) < boost::bind(&manager::subject::name, _2);
    
// syncro_manager::object::object(const Getter& g, const Setter& s, const QString& name, const QString& desc)
//     : getter_(g), setter_(s), name_(name), description_(desc)
// {}


struct syncro_manager::complete_object: object {
   
    complete_object(const Getter& getter, const Setter& setter, const QString& name, const QString& desc)
        : getter_(getter), setter_(setter), name_(name), description_(desc)
    {}

    virtual inline const QString& name() const { return name_; }
    virtual inline const QString& description() const { return description_; }

    virtual remote::group get() const { return getter_(); };
    virtual void put(const remote::group& gr) { setter_(gr); };
    
private:
    Getter getter_;
    Setter setter_;
    
    QString name_;
    QString description_;
};



struct gdocs_service: public service {

    gdocs_service()
        : service("gdocs", "gdocs service")
    {}
    
    service::Storage do_create(boost::shared_ptr<QSettings> settings) const {
        std::cerr<<"do_create"<<std::endl;
        service::Storage s = service::Storage(new gdocs( QString(), QString(), QString() ));
        std::cerr<<"do_create ok"<<std::endl;
        return s;
    }
};




syncro_manager::syncro_manager()
{
    services_.push_back( Service(new gdocs_service) );
}

const std::list<Service>& syncro_manager::services() const
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


service::Storage syncro_manager::create(const Service& service)
{
   base_settings main;

   base_settings::qsettings_p manager_settings = main.get_settings(manager_options::uid());

   QString dir = QFileInfo(manager_settings->fileName()).dir().path();
   
   main.register_file("plugin_uid", dir + "/services/" + service->caption() + "/plugin1", false);

   base_settings::qsettings_p p = main.get_settings("plugin_uid");

   p->setValue("123123", "!!!!!!!!!");
   p->sync();

   std::cerr<<"synced"<<std::endl;
   
   return service->create( main.get_settings("plugin_uid") );

   std::cerr<<"service created"<<std::endl;
   
}

syncro_manager::CompleteObject syncro_manager::promote(const remote::syncro_manager::Object& obj) const
{
    return CompleteObject(obj, static_cast<complete_object*>(obj.get()));
}

void syncro_manager::bind(const Object& obj, const service::Storage& storage)
{
    objects_[promote(obj)].push_back(storage);
}


void syncro_manager::sync(const Object& obj)
{
    task t(obj, objects_[promote(obj)], obj->get());
    tasks_.insert(t);
}

void syncro_manager::sync_impl()
{
    task& t = const_cast<task&>(*tasks_.begin());
    service::Storage& storage = t.storages.front();
    
    remote::action* action = storage->get(t.group.type());
    assert(connect(action, SIGNAL(loaded(const remote::object&)), SLOT(loaded(const remote::object&))));
    assert(connect(action, SIGNAL(error(const QString&)), SLOT(error(const QString&))));
    assert(connect(action, SIGNAL(finished()), SLOT(finished())));
    action->start();
}


void syncro_manager::loaded(const remote::group& obj)
{
    task& t = const_cast<task&>(*tasks_.begin());
    t.entries = remote::merge( obj.entries(), t.entries );
}

void syncro_manager::error(const QString& err)
{
    task& t = const_cast<task&>(*tasks_.begin());
    t.storages.pop_front();
}

void syncro_manager::finished()
{
    task& t = const_cast<task&>(*tasks_.begin());
    t.storages.pop_front();

    if (t.storages.empty())
    {
        t.object->put(remote::group(t.group.type(), t.entries));
        tasks_.erase(tasks_.begin());
    }
}

struct equal_name {
    typedef bool result_type;
    template <typename Pair>
    bool operator()(const Pair& pair, const QString& name)
    { return pair.first->name() == name; }
};

syncro_manager::Object syncro_manager::attach(const QString& name, const Getter& g, const Setter& s, const QString& desc)
{
    if (boost::find_if(objects_, boost::bind(equal_name(), _1, name)) == objects_.end() )
        throw std::runtime_error("Object with such name already attached!");

    CompleteObject object(new complete_object(g, s, name, desc));
    objects_.insert(std::make_pair(object, Storages()));
    
    return object;
}

void syncro_manager::detach(const remote::syncro_manager::Object& obj)
{
    if (objects_.erase(promote(obj)) == 0)
        throw std::runtime_error("No object with such name attached!");
}


};