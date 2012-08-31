
#include <iostream>

#include <boost/range/algorithm/find_if.hpp>
#include <boost/range/algorithm_ext/erase.hpp>

#include <QDir>
#include <QFileInfo>
#include <QSet>
#include <QSettings>
#include <QUuid>

#include "cl/syslog/syslog.h"
#include "common/qt_syslog.h"

#include "settings/settings.h"

#include "manager.h"
#include "manager_impl.h"
#include "task.h"
#include "settings.h"

#include "backends/gdocs/gdocs.h"
#include "backends/fileservice/fileservice.h"

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

#define THROW_IF_EQUAL(it, end, msg) if ((it) == (end)) throw std::runtime_error(#msg);
#define THROW_IF_NOT_EQUAL(it, end, msg) if ((it) != (end)) throw std::runtime_error(#msg);

namespace remote {

///const cast pointer to refference(to convert "key" objects to "useful" objects)
template <typename T>
inline T& cast(const boost::shared_ptr<const T>& ptr) {
    return *const_cast<T*>(ptr.get());
}

    
QString con_str(const QString& str1, const QString& str2) {
    return str1 + "-" + str2;
}

//FIXME remove from there
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

    std::auto_ptr<QVariantMap> configure(const QVariantMap& settings) const{}
};

syncro_manager::syncro_manager()
    : p_(new Pimpl)
{
    p_->services_uid = con_str(manager_options::uid(), "services");
    base_settings().register_sub_group(p_->services_uid, s_services, manager_options::uid());
    
    LOG_DEBUG << "remote services config registered at uid='%1' file='%2'", p_->services_uid, base_settings().get_settings(p_->services_uid)->fileName();
    
    register_service(Service(new fileservice));
}

void syncro_manager::register_service(const Service& srv)
{
    Pimpl::Services::iterator it; bool inserted;
    std::tr1::tie(it, inserted) = p_->srv_list.insert(srv);
    
    THROW_IF_EQUAL(inserted, false, "This service already registered");

    const QString service_uid = uuid(srv);
    const QString storage_list_uid = st_list_uid(service_uid);

    base_settings().register_sub_group(service_uid, srv->caption(), p_->services_uid);
    base_settings().register_sub_group(storage_list_uid, s_storages, service_uid);

    LOG_INFO << "new remote service registered '%1' UID:'%2'" , srv->caption(), service_uid;
    LOG_DEBUG << "new remote service file='%1'", base_settings().get_settings(service_uid)->fileName();
}

const syncro_manager::Pimpl::Services& syncro_manager::services() const
{
    return p_->srv_list;
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
    if (storage)
        return p_->objects(storage);
    else
        return p_->obj_list;
}

template <typename Service>
void setup_generic_settings(const QString& storage_uid, const QString& name, const QString& storage_list_uid, const Service& srv)
{
    base_settings::qsettings_p generic = base_settings().register_sub_group(storage_uid, name, storage_list_uid);

    generic->setValue(s_service_tag, srv->caption());
    generic->setValue(s_uuid_tag, storage_uid);
    generic->setValue(s_name_tag, name);
}

syncro_manager::Storage syncro_manager::create(const Service& srv, const QString& name, const QVariantMap& settings, QString storage_uid)
{
    const bool load = !storage_uid.isEmpty();
    storage_uid = (storage_uid.isEmpty())
        ? QUuid::createUuid().toString()
        : storage_uid;
        
    const QString service_uid = uuid(srv);
    const QString storage_list_uid = st_list_uid(service_uid);
    const QString storage_instance_uid = st_instance_uid(storage_uid);

    if (!load && manager_options().storages().contains(name)) throw std::runtime_error("storage with this name already exists");
    
    setup_generic_settings(storage_uid, name, storage_list_uid, srv);

    base_settings::qsettings_p storage_settings = base_settings().register_sub_group(storage_instance_uid, "data", storage_uid);
    storage_settings = fill_settings(storage_settings, settings);
    
    service::Storage storage = srv->create(storage_settings);
    manager_options().storages_set(manager_options().storages() << name);

    Pimpl::StorageData::iterator new_it = p_->st_data.insert(Pimpl::storage_data(srv, storage, storage_uid)).first;
    
    emit storage_changed(new_it->storage, Storage());
    
    return new_it->storage;
}

void syncro_manager::remove(const Storage& storage)
{
    unbind(storage);
    
    Pimpl::st_iterator<Storage>::type it = p_->st_find(storage);
    base_settings::qsettings_p settings = base_settings().get_settings(it->storage_uid);

    QStringList storages = manager_options().storages();
    storages.removeAll(settings->value(s_name_tag).toString());
    manager_options().storages_set(storages);
    
    // Official Qt: 
    // void QSettings::clear() - Removes all entries in the primary location associated to this QSettings object.
    // If you only want to remove the entries in the current group(), use remove("") instead.
    settings->remove("");
    settings->sync();
    
    p_->st_data.get<Storage>().erase(it);
    emit storage_changed(Storage(), storage);        
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

syncro_manager::Object syncro_manager::attach(const QString& name, const Getter& g, const Setter& s, const QString& desc, const QIcon& icon)
{
    Pimpl::Objects::iterator it = boost::find_if(p_->obj_list, boost::bind(&object::name, _1) == name);
    THROW_IF_NOT_EQUAL(it, p_->obj_list.end(), "Object with such name already attached!");

    it = p_->obj_list.insert(Object(new object(g, s, name, desc, icon))).first;

    manager_options().objects_set((manager_options().objects().toSet() << name).toList());
    
    object_attached(*it);
    
    base_settings::qsettings_p settings = base_settings().get_settings(manager_options::uid());
    settings_group gr(settings, "objetcs/" + name);
    QStringList binded = settings->value("binded", QStringList()).toStringList();
    BOOST_FOREACH(QString st_name, binded) {
        
        Storage st;
        BOOST_FOREACH(Storage storage, storages()) {
            if (this->name(storage) == st_name)
            {
                st = storage;
            }
        }
        
        if (st.get())
        {
            bind(*it, st);
        }
    }
    
    return *it;
}

void syncro_manager::detach(const Object& obj)
{
    unbind(obj);
    p_->obj_list.erase(obj);
    
    QStringList objects = manager_options().objects();
    objects.removeAll(obj->name());
    manager_options().objects_set(objects);
    
    object_detached(obj);
}


void syncro_manager::bind(const Object& obj, const Storage& storage)
{
    Pimpl::ObjectsData::iterator it; bool inserted;
    std::tr1::tie(it, inserted) = p_->obj_data.insert(Pimpl::objects_data(obj, storage));
    
    THROW_IF_EQUAL(inserted, false, "This already binded to this storage");
    
    base_settings::qsettings_p settings = base_settings().get_settings(manager_options::uid());
    settings_group gr(settings, "objetcs/" + obj->name());
    
    QStringList binded = settings->value("binded", QStringList()).toStringList();
    binded = (binded.toSet() << name(storage)).toList();
    settings->setValue("binded", binded);
        

    LOG_DEBUG << "object '%1' binded to '%2'", obj->name(), p_->st_find<Storage>(storage)->service->caption();
    emit object_changed(obj);    
}

void syncro_manager::unbind(const Object& obj)
{
    boost::erase(p_->obj_data.get<Object>(),
        make_iterator_range(p_->obj_data.get<Object>().equal_range(obj)));

    base_settings::qsettings_p settings = base_settings().get_settings(manager_options::uid());
    settings->remove("objetcs/" + obj->name());
    
    emit object_changed(obj);
}


void syncro_manager::unbind(const Storage& storage)
{
    const QString storage_name = name(storage);
    boost::erase(p_->obj_data.get<Storage>(),
        make_iterator_range(p_->obj_data.get<Storage>().equal_range(storage)));

    base_settings::qsettings_p settings = base_settings().get_settings(manager_options::uid());
    
    BOOST_FOREACH(const Object& object, p_->objects(storage)) {
        settings_group gr(settings, "objetcs/" + object->name());
        QStringList binded = settings->value("binded", QStringList()).toStringList();
        binded.removeAll(storage_name);
        settings->setValue("binded", binded);
        
        emit object_changed(object);
    }
}

void syncro_manager::unbind(const remote::syncro_manager::Object& obj, const remote::syncro_manager::Storage& storage)
{
    Pimpl::obj_iterator<Pimpl::objects_data>::type it = p_->obj_find(Pimpl::objects_data(obj, storage));
    
    base_settings::qsettings_p settings = base_settings().get_settings(manager_options::uid());
    settings_group gr(settings, "objetcs/" + obj->name());
    
    QStringList binded = settings->value("binded", QStringList()).toStringList();
    binded.removeAll(name(storage));
    settings->setValue("binded", binded);
    
    p_->obj_data.get<Pimpl::objects_data>().erase(it);
    
    emit object_changed(obj);
}


void syncro_manager::load()
{
    base_settings::qsettings_p all_services_cfg = base_settings().get_settings(p_->services_uid);
    
    BOOST_FOREACH(const QString& service, all_services_cfg->childGroups()) {
        
        LOG_DEBUG << "trying to load <" << service << "> service";
        
        Pimpl::Services::iterator it = boost::find_if(p_->srv_list, boost::bind(&remote::service::caption, _1) == service);
            
        if (it == p_->srv_list.end())
        {
            LOG_ERR << "remote service '%1' cannot be loaded", service;
            continue;
        }

        base_settings::qsettings_p service_cfg = settings(*it);

        LOG_INFO << "service '%1' loaded", service;

        settings_group storage_list_lock(service_cfg, s_storages);
        
        BOOST_FOREACH(const QString& storage_name, service_cfg->childGroups()) {
            try {
                LOG_DEBUG << "loading storage '%1'", storage_name;
                settings_group storage_desc_lock(service_cfg, storage_name);
                QVariantMap desc_settings = extract_settings(service_cfg);
            
                THROW_IF_NOT_EQUAL(desc_settings[s_service_tag].toString(), service, "invalid storage description[service]");
                THROW_IF_NOT_EQUAL(desc_settings[s_name_tag].toString(), storage_name, "invalid storage description[name]");
                THROW_IF_EQUAL(desc_settings[s_uuid_tag], QVariant(), "invalid storage description[uuid]");

                create(*it, storage_name, desc_settings[s_instance].toMap(), desc_settings[s_uuid_tag].toString());
                LOG_INFO << "storage '%1' loaded", storage_name;
            }
            catch (const std::exception& e)
            {
                LOG_ERR << "remote storage '%1' cannot be loaded: %2", storage_name, e.what();
            }
        }
    }
}

void syncro_manager::get(const remote::syncro_manager::Object& obj)
{
    THROW_IF_NOT_EQUAL(p_->active.find(obj), p_->active.end(), "Object alredy in process");

    boost::shared_ptr<task> gt = task_factory::create_get(obj, p_->storages(obj), obj->get());
    p_->active.insert(std::make_pair(obj, gt));

    try {
        connect(gt.get(),
                SIGNAL(completed(const syncro_manager::Object&, const remote::group&)),
                SLOT(completed(const syncro_manager::Object&, const remote::group&)));
        
        connect(gt.get(), SIGNAL(error(const QString&)), SLOT(error(const QString&)));
        connect(gt.get(), SIGNAL(finished()), SLOT(finished()));
        
        gt->start();
    }
    catch (const std::exception& e) {
        LOG_ERR << "can't start task for object '%1'", obj->name();
        p_->active.erase(obj);
    }
}

void syncro_manager::put(const remote::syncro_manager::Object& obj)
{
    THROW_IF_NOT_EQUAL(p_->active.find(obj), p_->active.end(), "Object alredy in process");

    boost::shared_ptr<task> gt = task_factory::create_put(obj, p_->storages(obj), obj->get());
    p_->active.insert(std::make_pair(obj, gt));

    try {
        connect(gt.get(),
                SIGNAL(completed(const syncro_manager::Object&)),
                SLOT(completed(const syncro_manager::Object&)));
        
        connect(gt.get(), SIGNAL(error(const QString&)), SLOT(error(const QString&)));
        connect(gt.get(), SIGNAL(finished()), SLOT(finished()));
        gt->start();
    }
    catch (const std::exception& e) {
        LOG_ERR << "can't start task for object '%1'", obj->name();
        p_->active.erase(obj);
    }
}

void syncro_manager::sync(const Object& obj)
{
    THROW_IF_NOT_EQUAL(p_->active.find(obj), p_->active.end(), "Object alredy in process");

    boost::shared_ptr<task> gt(task_factory::create_sync(obj, p_->storages(obj), obj->get()));
    
    p_->active.insert(std::make_pair(obj, gt));

    try {
        connect(gt.get(),
                SIGNAL(completed(const syncro_manager::Object&, const remote::group&)),
                SLOT(completed(const syncro_manager::Object&, const remote::group&)));
        
        connect(gt.get(), SIGNAL(error(const QString&)), SLOT(error(const QString&)));
        connect(gt.get(), SIGNAL(finished()), SLOT(finished()));
        
        gt->start();
    }
    catch (const std::exception& e) {
        LOG_ERR << "can't start task for object '%1'", obj->name();
        p_->active.erase(obj);
    }
}

void syncro_manager::error(const QString& err)
{
    std::cerr << "Error in syncing:" << err.toLocal8Bit().constData() << std::endl;
}

void syncro_manager::completed(const syncro_manager::Object& obj, const remote::group& group)
{
    cast(obj).put(group);
}

void syncro_manager::completed(const syncro_manager::Object& obj)
{
    
}

void syncro_manager::finished()
{
    task* t = qobject_cast<task*>(sender());
    const Object& obj = t->object();
    
    p_->active.erase(obj);
}

QString syncro_manager::name(const Storage& storage) const
{
    return settings(storage)[s_name_tag].toString();
}



};
