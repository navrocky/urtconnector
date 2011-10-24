

#include <boost/bind.hpp>

#include <backends/gdocs/gdocs.h>
#include <manager.h>


using namespace std;
using namespace boost;
 
namespace remote {

const manager::ObjectCompare c_compare = boost::bind(&manager::object::name, _1) < boost::bind(&manager::object::name, _2);
    
manager::object::object(const Getter& g, const Setter& s, const QString& name, const QString& desc)
    : getter_(g), setter_(s), name_(name), description_(desc)
{}


manager::Storage create_gdocs(){
    return manager::Storage(new gdocs( QString(), QString(), QString() ));
}

manager::manager()
//     : objects_(c_compare)
{
    
    factory_["gdoc"] = create_gdocs;
}

boost::shared_ptr< manager::registrator > manager::reg(const remote::manager::object& o)
{
    std::pair<Objects::iterator, bool> res = objects_.insert(make_pair(new object(o), Storages()));
    assert(res.second);
 
    boost::function<void()> eraser = boost::bind<void>(&Objects::erase, boost::ref(objects_), res.first);
    return boost::shared_ptr<manager::registrator>(new registrator(eraser));
}


};