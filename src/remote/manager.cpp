
#include <iostream>


#include <boost/bind.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/find_if.hpp>

#include <QFileInfo>
#include <QDir>
#include <QSet>
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

using namespace boost::multi_index;

static const QString s_caption = "service_caption";

SYSLOG_MODULE(syncro_manager);

#define SYNC_DEBUG LOG_DEBUG << " >> " << __FUNCTION__ << ": "

#define CHECK_ITERATOR(it, end, msg) if ((it) == (end)) throw std::runtime_error(#msg);

struct caption {};

namespace remote {

template<class KeyExtractor1,class KeyExtractor2>
struct key_from_key
{
public:
    typedef typename KeyExtractor1::result_type result_type;

    key_from_key(
        const KeyExtractor1& key1_=KeyExtractor1(),
        const KeyExtractor2& key2_=KeyExtractor2()):
        key1(key1_),key2(key2_)
    {}

    template<typename Arg>
    result_type operator()(Arg& arg)const
    {
        return key1(key2(arg));
    }

private:
    KeyExtractor1 key1;
    KeyExtractor2 key2;
};


struct syncro_manager::Pimpl {
    
    typedef std::set<Service> Services;
    typedef std::set<Storage> Storages;
    
    struct service_data {
        Service service;
        Storage storage;
        
        service_data(Service se, Storage st) : service(se), storage(st) {}
    };    
    
    struct objects_data {
        Object object;
        Storage storage;
        
        objects_data(Object o, Storage st) : object(o), storage(st) {}
    };   

    typedef multi_index_container<
        service_data,
        indexed_by<
            ordered_non_unique<
                tag<Service>,
                BOOST_MULTI_INDEX_MEMBER(service_data, Service, service)
            >,
            ordered_unique<
                tag<Storage>,
                BOOST_MULTI_INDEX_MEMBER(service_data, Storage, storage)
            >,
            ordered_unique<
                tag<caption>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(remote::service, const QString&, caption),
                    BOOST_MULTI_INDEX_MEMBER(service_data, Service, service)
                >
            >
        
        >
    > ServiceData;
    
    typedef multi_index_container<
        objects_data,
        indexed_by<
            ordered_non_unique<
                tag<Object>, BOOST_MULTI_INDEX_MEMBER(objects_data, Object, object)
            >,
            ordered_non_unique<
                tag<Storage>, BOOST_MULTI_INDEX_MEMBER(objects_data, Storage, storage)
            >,
            ordered_non_unique<
                tag<caption>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(syncro_manager::object, const QString&, name),
                    BOOST_MULTI_INDEX_MEMBER(objects_data, Object, object)
                >
            >            
        >
    > ObjectsData;

    ServiceData srv_data;     
    ObjectsData obj_data;
    
//     boost::fusion::map<
//         boost::fusion::pair<ServiceData, ServiceData>,
//         boost::fusion::pair<ObjectsData, ObjectsData> >
//     multi_data_;
//     
//     template <typename MultiType>
//     MultiType& data() { return boost::fusion::at_key<MultiType>(multi_data_); }
//     
//     template <typename MultiType>
//     const MultiType& data() const { return boost::fusion::at_key<MultiType>(multi_data_); }
    
    
    template <typename Tag>
    struct srv_iterator {
        typedef typename ServiceData::index<Tag>::type::iterator type;
    };
    
    template <typename Tag>
    struct obj_iterator {
        typedef typename ObjectsData::index<Tag>::type::iterator type;
    };
    
    template <typename Tag>
    typename srv_iterator<Tag>::type srv_begin() { return srv_data.get<Tag>().begin(); }
    
    template <typename Tag>
    typename srv_iterator<Tag>::type srv_end() { return srv_data.get<Tag>().end(); }
    
    template <typename Tag, typename Value>
    typename srv_iterator<Tag>::type srv_find(const Value& v) { return srv_data.get<Tag>().find(v); }
    
    template <typename Tag>
    typename obj_iterator<Tag>::type obj_begin() { return obj_data.get<Tag>().begin(); }
    
    template <typename Tag>
    typename obj_iterator<Tag>::type obj_end() { return obj_data.get<Tag>().end(); }
    
    template <typename Tag, typename Value>
    typename obj_iterator<Tag>::type obj_find(const Value& v) { return obj_data.get<Tag>().find(v); }

    
    QString services_uid;
    Services registered_services_;
    
    
    const Services& services() const {
        return registered_services_;
    }
    
    Storages storages() const {
        return Storages(
            make_transform_iterator(srv_data.begin(), boost::bind(&service_data::storage, _1)),
            make_transform_iterator(srv_data.end(), boost::bind(&service_data::storage, _1))
        );
    }
    
    Storages storages(const Object& obj) const {
        return Storages(
            make_transform_iterator(obj_data.lower_bound<Object>(obj), boost::bind(&objects_data::storage, _1)),
            make_transform_iterator(obj_data.upper_bound<Object>(obj), boost::bind(&objects_data::storage, _1))
        );
    }
    
    std::list<Object> objects() const {
        return std::list<Object>(
            boost::make_transform_iterator(obj_data.begin(), boost::bind(&objects_data::object, _1)),
            boost::make_transform_iterator(obj_data.end(), boost::bind(&objects_data::object, _1))
        );
    }
    
};








    

    

// struct test{
//     typedef std::map<syncro_manager::Service, std::list<syncro_manager::Storage> > StoragesByService;
//     typedef std::map<syncro_manager::Storage, syncro_manager::Service > ServiceByStorage;
//     typedef std::map<std::string, syncro_manager::Service> ServiceByName;
//     
//     StoragesByService stse_;
//     ServiceByStorage sest_;
//     ServiceByName sen_;
//     
//     void insert(const std::string& name, syncro_manager::Service se, syncro_manager::Storage st){
//         stse_[se].push_back(st);
//         sest_[st] = se;
//         sen_[name] = se;
//     }
//     
//     const syncro_manager::Service& service(const std::string& name) const {
//         const ServiceByName::const_iterator it = sen_.find(name);
//         if (it == sen_.end()) throw std::runtime_error("no service for such name");
//         return it->second;
//     }
//     
//     const syncro_manager::Service& service(const syncro_manager::Storage& storage) const {
//         const ServiceByStorage::const_iterator it = sest_.find(storage);
//         if (it == sest_.end()) throw std::runtime_error("no service for such storage");
//         return it->second;
//     }
//     
//     const std::list<syncro_manager::Storage>& storages(const syncro_manager::Service& service) const {
//         const ServiceByStorage::const_iterator it = sest_.find(storage);
//         if (it == sest_.end()) throw std::runtime_error("no service for such storage");
//         return it->second;
//     }
//     
//    
//     
// };

template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
    return *const_cast<T*>(ptr.get());
}

struct gdocs_service: public service {

    gdocs_service()
        : service("gdocs", "gdocs service")
    {}
    
    service::Storage do_create(const boost::shared_ptr<QSettings>& settings) const {
        if (!settings->contains("mail") || !settings->contains("password"))
            throw std::runtime_error("settings not valid");
            
        service::Storage s = service::Storage(new gdocs( settings->value("main").toString(), settings->value("password").toString(), "urt" ));
        return s;
    }
};

struct gdocs_service2: public service {

    gdocs_service2()
        : service("gdocs2", "gdocs service2")
    {}
    
    service::Storage do_create(const boost::shared_ptr<QSettings>& settings) const {
        if (!settings->contains("mail") || !settings->contains("password"))
            throw std::runtime_error("settings not valid");
            
        service::Storage s = service::Storage(new gdocs( settings->value("main").toString(), settings->value("password").toString(), "urt" ));
        return s;
    }
};


syncro_manager::syncro_manager()
    : p_(new Pimpl)
{
    p_->services_uid = base_settings().get_settings(manager_options::uid())->fileName() + "services";
    base_settings().register_sub_group(p_->services_uid, "services", manager_options::uid());
    
    p_->registered_services_.insert(Service(new gdocs_service));
}

syncro_manager::Pimpl::Services syncro_manager::services() const
{
    return p_->services();
}

syncro_manager::Pimpl::Storages syncro_manager::storages() const
{
    return p_->storages();
}

std::list<syncro_manager::Object> syncro_manager::objects() const
{
    return p_->objects();
}



syncro_manager::Storage syncro_manager::create(const Service& srv, const QString& name, const QVariantMap& settings)
{
    CHECK_ITERATOR(p_->services().find(srv), p_->services().end(), "so such service registered");
    
    remote::service& service = cast(srv);

    manager_options().storages_set((manager_options().storages().toSet() << name).toList());
    
    base_settings main;
    main.register_sub_group(p_->services_uid + name, name, p_->services_uid);
    
    LOG_DEBUG << "NEW GUID:" << p_->services_uid + name;
    
    base_settings::qsettings_p s = fill_settings(main.get_settings(p_->services_uid + name), settings);
    s->setValue(s_caption, service.caption());
    
    Storage storage = p_->srv_data.insert(Pimpl::service_data(srv, service.create(s))).first->storage;


//FIXME ============ remove ==============
    Pimpl::srv_iterator<Storage>::type it1, it2;
    tr1::tie(it1, it2) = get<Storage>(p_->srv_data).equal_range(storage);
    assert(std::distance(it1, it2) == 1);
//FIXME ==================================
    
    SYNC_DEBUG << "storage created";

    return storage;
}

void syncro_manager::bind(const Object& obj, const Storage& storage)
{
    SYNC_DEBUG << p_->obj_data.size();
    p_->obj_data.insert(Pimpl::objects_data(obj, storage));
    SYNC_DEBUG << p_->obj_data.size();
    
//FIXME ============ remove ==============
    Pimpl::obj_iterator<Object>::type it1, it2;
    tr1::tie(it1, it2) = get<Object>(p_->obj_data).equal_range(obj);
    
    SYNC_DEBUG << "distance: " << std::distance(it1, it2);
    
    assert(std::distance(it1, it2) == 1);
    
    Pimpl::obj_iterator<Storage>::type it11, it22;
    tr1::tie(it11, it22) = get<Storage>(p_->obj_data).equal_range(storage);
    assert(std::distance(it11, it22) == 1);
    
//FIXME ==================================
    
    SYNC_DEBUG << "object binded, name=" << obj->name();    
}

void syncro_manager::load()
{
    QStringList valid_storages;
    manager_options mo;
    
    BOOST_FOREACH(const QString& storage, mo.storages()) {
        base_settings::qsettings_p storage_cfg = base_settings().register_sub_group(p_->services_uid + storage, storage, p_->services_uid);
        if (!storage_cfg->allKeys().contains(s_caption)) continue;

        try {
            Pimpl::srv_iterator<caption>::type srv = p_->srv_find<caption>(storage_cfg->value(s_caption).toString());
            if (srv == p_->srv_end<caption>()) throw std::runtime_error("no such service registered");
            
            LOG_DEBUG << "loading storage " << storage_cfg->value(s_caption).toString();
            p_->srv_data.insert(Pimpl::service_data(srv->service, cast(srv->service).create(storage_cfg)));
            valid_storages << storage;
        }
        catch (std::runtime_error& e) {
            LOG_ERR << "can't restore service %1: %2", storage_cfg->value(s_caption).toString(), e.what();
        }
    }
    
    if (valid_storages.isEmpty())
        mo.storages_reset();
    else
        mo.storages_set(valid_storages);
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
    sync_task& task = *tasks_.insert(tasks_.end(), sync_task(obj, p_->storages(obj), obj->get()));
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
    sync_task& task = *tasks_.insert(tasks_.end(), sync_task(obj, p_->storages(obj), obj->get()));
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
    if (p_->obj_find<caption>(name) != p_->obj_end<caption>())
        throw std::runtime_error("Object with such name already attached!");

    Object obj(new object(g, s, name, desc));
    p_->obj_data.insert(Pimpl::objects_data(obj, Storage()));
    
    return obj;
}

void syncro_manager::detach(const syncro_manager::Object& obj)
{
    Pimpl::obj_iterator<Object>::type it1, it2;
    boost::tuples::tie(it1,it2) = get<Object>(p_->obj_data).equal_range(obj);
    
    p_->obj_data.erase(it1, it2);
}

// remote::service& syncro_manager::find_service(const QString& caption)
// {
//     LOG_ERR << "caption:" << caption;
//     std::list<Service>::iterator srv = boost::find_if(services_, boost::bind(&service::caption, _1) == caption);
//     LOG_DEBUG << caption;
//     if (srv == services_.end())
//         throw std::runtime_error("No service with such caption");
//     
//     return cast(*srv);
// }



};