

#include <boost/bind.hpp>

#include <backends/gdocs/gdocs.h>
#include <manager.h>

#include "storage.h"

using namespace std;
using namespace boost;
 
namespace remote {

// const manager::ObjectCompare c_compare = boost::bind(&manager::subject::name, _1) < boost::bind(&manager::subject::name, _2);
    
manager::subject::subject(const Getter& g, const Setter& s, const QString& name, const QString& desc)
    : getter_(g), setter_(s), name_(name), description_(desc)
{}


struct gdocs_service: remote::service {

    
    service::Storage do_create() const {
        service::Storage(new gdocs( QString(), QString(), QString() ));
    }
};

manager::manager()
//     : objects_(c_compare)
{
    services_.push_back( Service(new gdocs_service) );
}

const std::list<Service>& manager::services() const
{
    Service ss;
    ConstService s = ss;
    
    return services_;
    
}

service::Storage manager::create(const Service& service)
{
   service->create();
}

void manager::bind(const Subject& obj, const service::Storage& storage)
{
    subjects_[obj].push_back(storage);
}

/*
struct merger: public QObject {

    merger(const remote::object& initial, const manager::Object& sg)
         : obj_(initial)
         , merged_(initial.entries())
         , setter_getter(sg)
        {}

void insert(remote::action* action) {
        actions_.push_back(action);
        connect(action, SIGNAL(loaded(const object&)), this, SLOT(loaded(const object&)));
        connect(action, SIGNAL(finished()), this, SLOT(finished()));
}

void start() {
        BOOST_FOREACH(remote::action* action, std::list<remote::action*>(actions_)) {
                action->start();
        }
}

public Q_SLOTS:
    void loaded(const remote::object& obj) {
        merged_ = remote::merge( obj.entries(), merged_ );
    }

    void finished() {
        actions_.remove( static_cast<action*>(sender()) );

        if (actions_.empty())
        {
                setter_getter->put(remote::object(obj_.type(), merged_));
        }
    }
    
private:
    remote::object obj_;
    const manager::Object& setter_getter;
    remote::object::Entries merged_;

    std::list<remote::action*> actions_;
};*/

void manager::sync(const Subject& subject)
{
    queued q(subject, subjects_[subject], subject->get());

    sync_queue_.push_back(q);
  
}

void manager::sync_impl()
{
    queued& q = sync_queue_.front();
    service::Storage& storage = q.storages.front();
    
    remote::action* action = storage->get(q.object.type());
    action->start();
    
    
}


void manager::loaded(const remote::object& obj)
{
    
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