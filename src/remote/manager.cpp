

#include <boost/bind.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include <backends/gdocs/gdocs.h>
#include <manager.h>

#include "storage.h"

using namespace std;
using namespace boost;
 
namespace remote {

// const manager::ObjectCompare c_compare = boost::bind(&manager::subject::name, _1) < boost::bind(&manager::subject::name, _2);
    
syncro_manager::object::object(const Getter& g, const Setter& s, const QString& name, const QString& desc)
    : getter_(g), setter_(s), name_(name), description_(desc)
{}


struct gdocs_service: public service {

    gdocs_service()
        : service("gdocs", "gdocs service")
    {}
    
    service::Storage do_create() const {
        service::Storage(new gdocs( QString(), QString(), QString() ));
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

// typedef boost::transform_iterator<typeof()UnaryFunc, Iterator>(it, fun)

std::list<syncro_manager::Object> syncro_manager::objects() const
{
    return std::list<syncro_manager::Object>(
        boost::make_transform_iterator(objects_.begin(), boost::bind(&Objects::value_type::first, _1)),
        boost::make_transform_iterator(objects_.end(), boost::bind(&Objects::value_type::first, _1))
    );
}


service::Storage syncro_manager::create(const Service& service)
{
   service->create();
}

void syncro_manager::bind(const Object& obj, const service::Storage& storage)
{
    objects_[obj].push_back(storage);
}


void syncro_manager::sync(const Object& obj)
{
    task t(obj, objects_[obj], obj->get());
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



// boost::shared_ptr< manager::registrator > manager::reg(const remote::manager::object& o)
// {
//     std::pair<Objects::iterator, bool> res = objects_.insert(make_pair(new object(o), Storages()));
//     assert(res.second);
//  
//     boost::function<void()> eraser = boost::bind<void>(&Objects::erase, boost::ref(objects_), res.first);
//     return boost::shared_ptr<manager::registrator>(new registrator(eraser));
// }


};