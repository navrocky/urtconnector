
#include <iostream>


//FIXME cleanup headers
#include <boost/foreach.hpp>
#include <boost/bind.hpp>
#include <boost/fusion/container/map.hpp>
#include <boost/fusion/include/at_key.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/multi_index/global_fun.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index_container.hpp>
// #include <boost/range/algorithm/find.hpp>
 #include <boost/range/algorithm/find_if.hpp>
// #include <boost/range/algorithm/set_algorithm.hpp>
#include <boost/range/algorithm_ext/erase.hpp>
#include <boost/multi_index/hashed_index.hpp>

#include <QFileInfo>
#include <QDir>
#include <QSet>
#include <QSettings>
#include <QUuid>

#include <backends/gdocs/gdocs.h>
#include "manager.h"

#include "settings/settings.h"
#include "settings.h"

#include "storage.h"

#include "cl/syslog/syslog.h"
#include "common/qt_syslog.h"
#include "backends/fileservice/fileservice.h"

using namespace std;
using namespace boost;

using namespace boost::multi_index;

static const QString s_service_tag = "storage_service";
static const QString s_uuid_tag = "storage_uuid";
static const QString s_name_tag = "storage_name";

static const QString s_services = "services";
static const QString s_storages = "storages";
static const QString s_instance = "instance";

inline QString st_instance_uid(const QString& storage_uid) {
	return storage_uid + s_instance;
}

inline QString st_list_uid(const QString& service_uid) {
	return service_uid + s_storages;
}



SYSLOG_MODULE(syncro_manager);

#define SYNC_DEBUG LOG_DEBUG << " >> " << __FUNCTION__ << ": "

#define THROW_IF_EQUAL(it, end, msg) if ((it) == (end)) throw std::runtime_error(#msg);
#define THROW_IF_NOT_EQUAL(it, end, msg) if ((it) != (end)) throw std::runtime_error(#msg);

namespace remote {

///const cast pointer to refference(to convert "key" objects to "useful" objects)
template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
    return *const_cast<T*>(ptr.get());
}

    
template<class KeyExtractor1,class KeyExtractor2>
struct key_from_key
{
public:
    typedef typename KeyExtractor1::result_type result_type;

    key_from_key(
        const KeyExtractor1& key1_ = KeyExtractor1(),
        const KeyExtractor2& key2_ = KeyExtractor2()):
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
	typedef std::set<Object>  Objects;
    
    struct storage_data {
        const Service service;
        const Storage storage;
		const QString storage_uid;

        //mutable QString service_uid;
        
        storage_data(Service se, Storage st, const QString& uid)
			: service(se), storage(st), storage_uid(uid){}
        
        bool operator == (const storage_data& other) const {
            return storage == other.storage;
        }
    };    
    
    struct objects_data {
        Object object;
        Storage storage;
        
        objects_data(Object o, Storage st) : object(o), storage(st) {}
        
        bool operator == (const objects_data& other) const {
            return object == other.object && storage == other.storage;
        }
    };   

    struct hash {
        std::size_t operator()(storage_data const& data) const {
            return 
                reinterpret_cast<int>(data.service.get()) << 8 |
                reinterpret_cast<int>(data.storage.get()); 
        }
        
        std::size_t operator()(objects_data const& data) const {
            return 
                reinterpret_cast<int>(data.object.get()) << 8 |
                reinterpret_cast<int>(data.storage.get()); 
        }
    };

    typedef multi_index_container<
        storage_data,
        indexed_by<
			ordered_unique<
				tag<Storage>, BOOST_MULTI_INDEX_MEMBER(storage_data, const Storage, storage)
			>,
            ordered_non_unique<
                tag<Service>, BOOST_MULTI_INDEX_MEMBER(storage_data, const Service, service)
            >,
            ordered_non_unique<
                tag<QString>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(remote::service, const QString&, caption),
                    BOOST_MULTI_INDEX_MEMBER(storage_data, const Service, service)
                >
            >            
        >
    > StorageData;
    
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
                tag<QString>,
                key_from_key<
                    BOOST_MULTI_INDEX_CONST_MEM_FUN(syncro_manager::object, const QString&, name),
                    BOOST_MULTI_INDEX_MEMBER(objects_data, Object, object)
                >
            >,
            hashed_unique<
                tag<objects_data>,
                identity<objects_data>,
                hash
            >              
        >
    > ObjectsData;

	Services srv_list;
	Objects obj_list;

    StorageData st_data;     
    ObjectsData obj_data;

    /*! metafunction to get iterator type*/
    template <typename Tag>
    struct st_iterator {
        typedef typename StorageData::index<Tag>::type::iterator type;
    };
    
    /*! metafunction to get iterator type*/
    template <typename Tag>
    struct obj_iterator {
        typedef typename ObjectsData::index<Tag>::type::iterator type;
    };
    
    //  aliases
    
    template <typename Tag>
    typename st_iterator<Tag>::type st_begin() { return st_data.get<Tag>().begin(); }
    
    template <typename Tag>
    typename st_iterator<Tag>::type st_end() { return st_data.get<Tag>().end(); }
    
    template <typename Tag>
    typename st_iterator<Tag>::type st_find(const Tag& key) { return st_data.get<Tag>().find(key); }
    
    template <typename Tag>
    typename obj_iterator<Tag>::type obj_begin() { return obj_data.get<Tag>().begin(); }
    
    template <typename Tag>
    typename obj_iterator<Tag>::type obj_end() { return obj_data.get<Tag>().end(); }
    
    template <typename Tag>
    typename obj_iterator<Tag>::type obj_find(const Tag& key) { return obj_data.get<Tag>().find(key); }

	Services services() const {
		return srv_list;
	}
    
    template <typename Tag>
    Services services(const Tag& key) const {
        return Services (
            make_transform_iterator(st_data.get<Tag>().lower_bound(key), boost::bind(&storage_data::service, _1)),
            make_transform_iterator(st_data.get<Tag>().upper_bound(key), boost::bind(&storage_data::service, _1))
        );
    }
    
    Storages storages() const {
        Storages st(
            make_transform_iterator(st_data.begin(), boost::bind(&storage_data::storage, _1)),
            make_transform_iterator(st_data.end(), boost::bind(&storage_data::storage, _1))
        );
        st.erase(Storage());
        return st;
    }
    
    template <typename Tag>
    Storages storages(const Tag& key) const {
        Storages st (
            make_transform_iterator(st_data.get<Tag>().lower_bound(key), boost::bind(&storage_data::storage, _1)),
            make_transform_iterator(st_data.get<Tag>().upper_bound(key), boost::bind(&storage_data::storage, _1))
        );
        st.erase(Storage());
        return st;
    }
    
    Storages storages(const Object& obj) const {
        Storages st (
            make_transform_iterator(obj_data.get<Object>().lower_bound(obj), boost::bind(&objects_data::storage, _1)),
            make_transform_iterator(obj_data.get<Object>().upper_bound(obj), boost::bind(&objects_data::storage, _1))
        );
        st.erase(Storage());
        return st;
    }
    
    std::set<Object> objects(const Storage& storage = Storage()) const {
        if (storage)
        {
            return std::set<Object>(
                boost::make_transform_iterator(obj_data.get<Storage>().lower_bound(storage), boost::bind(&objects_data::object, _1)),
                boost::make_transform_iterator(obj_data.get<Storage>().upper_bound(storage), boost::bind(&objects_data::object, _1))
            );
        }
        else
        {
            return std::set<Object>(
                boost::make_transform_iterator(obj_data.begin(), boost::bind(&objects_data::object, _1)),
                boost::make_transform_iterator(obj_data.end(), boost::bind(&objects_data::object, _1))
            );
        }
    }

    /*! UID to access "services" group of syncro_manager settings */
    QString services_uid;
};

struct syncro_manager::GetTask : public task {
    Object object;
    Storages storages;
    remote::group group;
    remote::action* action;
    remote::group::Entries entries;
    
    GetTask(const Object& o, const Storages& st, const remote::group& gr)
        : object(o), storages(st), group(gr), action(0){}
    
    void start() {
        Q_ASSERT(!storages.empty());

        action = cast(*storages.begin()).get(group.type());
        
        connect(action, SIGNAL(loaded(const remote::group&)), SLOT(loaded(const remote::group&)));
        connect(action, SIGNAL(error(QString)), SLOT(error(QString)));
        connect(action, SIGNAL(finished()), SLOT(finished()));
    }
    
    virtual void loaded(const remote::group& obj) {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
        
        entries = remote::merge(obj.entries(), entries);        
    }
    
    virtual void error(const QString& error) {
        SYNC_DEBUG << "Error:" << error;
    }
    
    virtual void finished() {
        Q_ASSERT(action == qobject_cast<remote::action*>(sender())); //sanity check
        Q_ASSERT(!storages.empty());
    
        storages.erase(storages.begin());

        if (storages.empty())
        {
            cast(object).put(remote::group(group.type(), entries));
        }
        else
        {
            start();
        }
    }
};

struct syncro_manager::SyncTask : public task {
    Object object;
    Storages get_storages;
    Storages put_storages;
    remote::group group;
    remote::action* action;
    remote::group::Entries entries;

    SyncTask(const Object& o, const Storages& st, const remote::group& gr)
        : object(o), get_storages(st), put_storages(st), group(gr), action(0)
        , entries(group.entries())
    {}

    void start() {
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
        SYNC_DEBUG << "Error:" << error;
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
             cast(object).put(group);
         }
         else
         {
             LOG_INFO << " <<< partial put";
             start_put();
         }
    }
};

QString con_str(const QString& str1, const QString& str2) {
    return str1 + "-" + str2;
}

struct gdocs_service: public service {

    gdocs_service()
        : service("gdocs", "gdocs service")
    {}
    
    service::Storage do_create(const boost::shared_ptr<QSettings>& settings) const {
        if (!settings->contains("mail") || !settings->contains("password"))
            throw std::runtime_error("settings not valid");
            
        service::Storage s = service::Storage(new gdocs( settings->value("mail").toString(), settings->value("password").toString(), "urt" ));
        return s;
    }

    QVariantMap configure(const QVariantMap& settings) const{}
};

///another service for testings
struct gdocs_service2: public service {

    gdocs_service2()
        : service("gdocs_testings", "gdocs service2")
    {}
    
    service::Storage do_create(const boost::shared_ptr<QSettings>& settings) const {
        if (!settings->contains("mail") || !settings->contains("password"))
            throw std::runtime_error("settings not valid");
            
        service::Storage s = service::Storage(new gdocs( settings->value("mail").toString(), settings->value("password").toString(), "urt" ));
        return s;
    }

    QVariantMap configure(const QVariantMap& settings) const{}
};


syncro_manager::syncro_manager()
    : p_(new Pimpl)
{
    p_->services_uid = con_str(manager_options::uid(), "services");
    base_settings().register_sub_group(p_->services_uid, s_services, manager_options::uid());
    
//    register_service(Service(new gdocs_service));
//    register_service(Service(new gdocs_service2));
    register_service(Service(new fileservice));

	LOG_DEBUG << "remote services config registered at uid='%1' file='%2'", p_->services_uid, base_settings().get_settings(p_->services_uid)->fileName();
}

void syncro_manager::register_service(const Service& srv)
{
    Pimpl::Services::iterator it; bool inserted;
    std::tr1::tie(it, inserted) = p_->srv_list.insert(srv);
    
    if (!inserted) throw std::runtime_error("This service already registered");

	const QString service_uid = con_str(p_->services_uid, srv->caption());
	const QString storage_list_uid = st_list_uid(service_uid);

    base_settings().register_sub_group(service_uid, srv->caption(), p_->services_uid);
    base_settings().register_sub_group(storage_list_uid, s_storages, service_uid);
    
    LOG_INFO << "new remote service registered '%1' UID:'%2'" , srv->caption(), service_uid;
	LOG_DEBUG << "new remote service file='%1'", base_settings().get_settings(service_uid)->fileName();
}



syncro_manager::Pimpl::Services syncro_manager::services() const
{
    return p_->services();
}

syncro_manager::Service syncro_manager::service(const Storage& storage) const
{
    Pimpl::st_iterator<Storage>::type it = p_->st_find<Storage>(storage);
    THROW_IF_EQUAL(it, p_->st_end<Storage>(), "no such storage registered");
    
    return it->service;
}


syncro_manager::Pimpl::Storages syncro_manager::storages(const Service& srv) const
{
    if (srv)
        return p_->storages(srv);
    else
        return p_->storages();
}

std::set<syncro_manager::Object> syncro_manager::objects(const Storage& storage) const
{
    return p_->objects(storage);
}

syncro_manager::Storage syncro_manager::create(const Service& srv, const QString& name, const QVariantMap& settings, QString storage_uid)
{
	Pimpl::Services::iterator it = p_->srv_list.find(srv);
    THROW_IF_EQUAL(it, p_->srv_list.end(), "no such service registered");

	storage_uid = (storage_uid.isEmpty())
		? QUuid::createUuid().toString()
		: storage_uid;

	const QString service_uid = con_str(p_->services_uid, srv->caption());
	const QString storage_list_uid = st_list_uid(service_uid);
	const QString storage_instance_uid = st_instance_uid(storage_uid);

    base_settings::qsettings_p storage_desc = base_settings().register_sub_group(storage_uid, name, storage_list_uid);
	LOG_DEBUG << "new remote storage-desc settings registered at uid='%1' file='%2'", storage_uid, storage_desc->fileName();

	storage_desc->setValue(s_service_tag, srv->caption());
	storage_desc->setValue(s_uuid_tag, storage_uid);
	storage_desc->setValue(s_name_tag, name);

	base_settings::qsettings_p storage_instance = base_settings().register_sub_group(storage_instance_uid, name, storage_uid);
	LOG_DEBUG << "new remote storage_instance settings registered at uid='%1' file='%2'", storage_instance, storage_instance->fileName();

    storage_instance = fill_settings(storage_instance, settings);
    
    service::Storage storage = srv->create(storage_instance);

    Pimpl::StorageData::iterator new_it = p_->st_data.insert(Pimpl::storage_data(srv, storage, storage_uid)).first;
    
    LOG_INFO << "new remote storage created: '%1'('%2') UID:'%3'", name, srv->caption(), new_it->storage_uid;

    emit storage_changed(new_it->storage, Storage());
    
    return new_it->storage;
}

void syncro_manager::remove(const Storage& storage)
{
    emit storage_changed(Storage(), storage);    
    unbind(storage);
    
    Pimpl::st_iterator<Storage>::type it = p_->st_find(storage);
    base_settings::qsettings_p settings = base_settings().get_settings(it->storage_uid);
    
    // Official Qt: 
    // void QSettings::clear() - Removes all entries in the primary location associated to this QSettings object.
    // If you only want to remove the entries in the current group(), use remove("") instead.
    settings->remove("");
    p_->st_data.get<Storage>().erase(it);
}


QVariantMap syncro_manager::settings(const Storage& storage) const
{
    Pimpl::st_iterator<Storage>::type it = p_->st_find<Storage>(storage);
    THROW_IF_EQUAL(it, p_->st_end<Storage>(), "no such storage registered");
    
    return extract_settings(base_settings().get_settings(it->storage_uid));
}

base_settings::qsettings_p syncro_manager::settings(const Service& srv)
{
	THROW_IF_EQUAL(base_settings::has_settings(uuid(srv)), false, "no settings for this storage!");
	return base_settings::get_settings(uuid(srv));
}

QString syncro_manager::uuid(const Service& srv) const
{
	THROW_IF_EQUAL(p_->srv_list.find(srv), p_->srv_list.end(), "no such service registered");
	return con_str(p_->services_uid, srv->caption());
}



void syncro_manager::bind(const Object& obj, const Storage& storage)
{
    Pimpl::ObjectsData::iterator it; bool inserted;
    std::tr1::tie(it, inserted) = p_->obj_data.insert(Pimpl::objects_data(obj, storage));
    
    if (!inserted) throw std::runtime_error("This already binded to this storage");
    
    //LOG_DEBUG << "object '%1' binded to '%2'", obj->name(), p_->st_find<Storage>(storage)->service->caption();
    
    emit object_changed(obj, obj);    
}

void syncro_manager::unbind(const Object& subject)
{
    emit object_changed(Object(), subject);
    
    boost::erase(p_->obj_data.get<Object>(),
        make_iterator_range(p_->obj_data.get<Object>().equal_range(subject)));
    
    p_->obj_data.insert(Pimpl::objects_data(subject, Storage()));
}


void syncro_manager::unbind(const Storage& storage)
{
    BOOST_FOREACH(const Object& object, p_->objects(storage)) {
        emit object_changed(object, object);
    }

    boost::erase(p_->obj_data.get<Storage>(),
        make_iterator_range(p_->obj_data.get<Storage>().equal_range(storage)));
}

void syncro_manager::unbind(const remote::syncro_manager::Object& subject, const remote::syncro_manager::Storage& storage)
{
    std::set<Object> all_objects = p_->objects(storage);
    
    Pimpl::obj_iterator<Pimpl::objects_data>::type it = p_->obj_find(Pimpl::objects_data(subject, storage));
    p_->obj_data.get<Pimpl::objects_data>().erase(it);
    
    BOOST_FOREACH(const Object& object, all_objects) {
        emit object_changed(object, object);
    }
}


void syncro_manager::load()
{
    base_settings::qsettings_p all_services_cfg = base_settings().get_settings(p_->services_uid);
    
    BOOST_FOREACH(const QString& service, all_services_cfg->childGroups()) {
        
		Pimpl::Services::iterator it = boost::find_if(p_->srv_list, boost::bind(&remote::service::caption, _1) == service);
			
        if (it == p_->srv_list.end())
        {
            LOG_WARN << "remote service '%1' cannot be loaded", service;
            continue;
        }

        base_settings::qsettings_p service_cfg = settings(*it);

        LOG_WARN << "service '%1' LOADED", service;

        settings_group storage_list_lock(service_cfg, s_storages);
        
        BOOST_FOREACH(const QString& storage_name, service_cfg->childGroups()) {
            LOG_WARN << "STORAGE '%1' LOADED", storage_name;
			
            settings_group storage_desc_lock(service_cfg, storage_name);
            QVariantMap desc_settings = extract_settings(service_cfg);
            
			THROW_IF_NOT_EQUAL(desc_settings[s_service_tag].toString(), service, "invalid storage description[service]");
			THROW_IF_NOT_EQUAL(desc_settings[s_name_tag].toString(), storage_name, "invalid storage description[name]");
			THROW_IF_EQUAL(desc_settings[s_uuid_tag], QVariant(), "invalid storage description[uuid]");
			
            create(*it, storage_name, desc_settings[s_instance].toMap(), desc_settings[s_uuid_tag].toString());
        }
    }
}



void syncro_manager::sync(const Object& obj)
{
    SyncTask* gt(new SyncTask(obj, p_->storages(obj), obj->get()));
    gt->start();
}

void syncro_manager::put(const remote::syncro_manager::Object& obj)
{
    sync(obj);
    return;

    if (boost::find_if(tasks_, boost::bind(&sync_task::object, _1) == obj) != tasks_.end())
    {
        return;        
    }
 
    sync_task& task = *tasks_.insert(tasks_.end(), sync_task(obj, p_->storages(obj), obj->get()));
    storage& storage = cast(task.current_storage());
    
    task.action = storage.put(obj->get());
    Q_ASSERT(connect(task.action, SIGNAL(saved()), SLOT(saved())));
    assert(connect(task.action, SIGNAL(error(const QString&)), SLOT(error(const QString&))));
    assert(connect(task.action, SIGNAL(finished()), SLOT(finished())));
    
    task.action->start();    
}


void syncro_manager::loaded(const remote::group& obj)
{
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);
    
    task.entries = remote::merge(obj.entries(), task.entries);
}

void syncro_manager::saved()
{
    SYNC_DEBUG << "save1";
    SYNC_DEBUG << "save2";
}


void syncro_manager::error(const QString& err)
{
    SYNC_DEBUG << "Error:" << err;
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);
}

void syncro_manager::finished()
{
    remote::action* action = qobject_cast<remote::action*>(sender());
    sync_task& task = get_task(boost::bind(&sync_task::action, _1) == action);    
    
    task.storages.erase(task.current_storage());

    if (task.storages.empty())
    {
        cast(task.object).put(group(task.group.type(), task.entries));
        tasks_.remove(task);
    }
}

syncro_manager::Object syncro_manager::attach(const QString& name, const Getter& g, const Setter& s, const QString& desc)
{
    if (p_->obj_find<QString>(name) != p_->obj_end<QString>())
        throw std::runtime_error("Object with such name already attached!");

    Object obj(new object(g, s, name, desc));
    bind(obj, Storage());
    return obj;
}

void syncro_manager::detach(const syncro_manager::Object& obj)
{
    Pimpl::obj_iterator<Object>::type it1, it2;
    boost::tuples::tie(it1,it2) = get<Object>(p_->obj_data).equal_range(obj);
    
    p_->obj_data.erase(it1, it2);
}

};
