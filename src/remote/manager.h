
#ifndef URT_REMOTE_MANAGER_H
#define URT_REMOTE_MANAGER_H

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <QUuid>

#include "settings/settings.h"

#include "remote.h"
#include "storage.h"

namespace remote {

/*! \brief Manager for all services and storages */
class syncro_manager: public QObject {
    Q_OBJECT
   
public:
    /*! Object for syncing: "bookmarks", "config", etc. */
    struct object;
    
    /*! Type of get-group callback */
    typedef boost::function<remote::group ()> Getter;
    
    /*! Type of set-group callback */
    typedef boost::function<void(const remote::group&)> Setter;
    
    /*! Type of access key to attached object */
    typedef boost::shared_ptr<const object> Object;
    
    /*! Type of access key to registered services*/
    typedef boost::shared_ptr<const remote::service> Service;
    
    /*! Type of access key to existing storage */
    typedef boost::shared_ptr<const storage> Storage;

    typedef std::set<Storage> Storages; 
    


    syncro_manager();

    /*! List of registered services */
    const std::set<Service>& services() const;

    /*! Get Service apropriate to this \p storage */
    Service service(const Storage& storage) const;
    
    
    /*! List of storages apropriate to this \p srv or \b All storages*/
    Storages storages(const Service& srv = Service()) const;
    
    /*! List of objects binded to this \p storage or \b All objects*/
    std::set<Object> objects(const Storage& storage = Storage()) const;

    /*! create new instance of storage provided by \p service */
    Storage create(const Service& service, const QString& name, const QVariantMap& settings, QString storage_uid = QString());
    
    void remove(const Storage& storage);
    
    /*! \brief get settings associated wth storage
     *  generic settings can be accessed directly: \example settings.value("storage_name").
     *  instance settings gropped in "data" group: \example settings.value("data/location").
     */
    QVariantMap settings(const Storage& storage) const;
   
    /*! attach callbacks to manager, and get object to interact
     * \p getter - callback to get data when synchronize needed
     * \p setter - callback to set data after synchronization
     * \p name - name must be uniqe within one manager \example "bookmarks"
     * \p desc - description
     */    
    Object attach(const QString& name, const Getter& g, const Setter& s, const QString& desc, const QIcon& icon = QIcon());
    
    /*! detacj object from manager */
    void detach(const Object&);


    /*! Bind \p subject to be synced with \p storage*/
    void bind(const Object& subject, const Storage& storage);
    
    /*! UnBind \p subject from all storages */
    void unbind(const Object& subject);
    
    /*! UnBind \p storage from all subjects */
    void unbind(const Storage& storage);
    
    void unbind(const Object& subject, const Storage& storage);
    
    
    
    void get(const Object& obj);

    void put(const Object& obj);
    
    /*! start syncronization */
    void sync(const Object& obj);
    
    

    /*! restore state of manager from config*/
    void load();

    QString name(const Storage& storage) const;

   
Q_SIGNALS:
    void storage_changed(remote::syncro_manager::Storage current, remote::syncro_manager::Storage previous);

    void object_attached(remote::syncro_manager::Object obj);
    void object_detached(remote::syncro_manager::Object obj);
    void object_changed(remote::syncro_manager::Object changed);
   
private Q_SLOTS:
    void completed(const syncro_manager::Object& obj, const remote::group& group);
    void completed(const syncro_manager::Object& obj);
    void finished();
    void error(const QString& err);

private:
    
    void register_service(const Service& srv);
    QString uuid(const Service& srv) const;
    base_settings::qsettings_p settings(const Service& srv);
   
private:
    
    
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    

    //TODO move to cpp and redesign

  /*  struct sync_task {
        
        sync_task(
            const Object& obj,
            const Storages& st,
            const remote::group& gr)
        : object(obj), storages(st), group(gr), entries(gr.entries())
        {}

        bool operator<(const sync_task& other) const { return object < other.object; }
        bool operator==(const sync_task& other) const { return object == other.object; }
        
        Storage current_storage() { 
            if (storages.empty()) throw std::runtime_error("no storages binded with this object");
            return *storages.begin();
        }
        
        Object object;
        Storages storages;
        remote::group group;
        remote::group::Entries entries;
        remote:: action* action;
    };
    
    std::list<sync_task> tasks_;
    
    template <typename Predicat>
    sync_task& get_task(const Predicat& p) {
        std::list<sync_task>::iterator it = std::find_if(tasks_.begin(), tasks_.end(), p);
        if (it == tasks_.end())
            throw std::runtime_error("no such task");
        
        return *it;
    }*/
    
    class task;
    struct task_factory;
    class get_task;
    class put_task;
    class sync_task;
};

/*! \brief This object represents one element of syncronization
 * For example: "bookmarks" or "profile"
 */
class syncro_manager::object {
public:

    inline const QIcon& icon() const { return icon_; }
    inline const QString& name() const { return name_; }
    inline const QString& description() const { return description_; }

    virtual remote::group get() const { return getter_(); };
    virtual void put(const remote::group& gr) { setter_(gr); };
    
private:
    
    friend class syncro_manager;

    object(const Getter& getter, const Setter& setter, const QString& name, const QString& desc, const QIcon& icon)
        : getter_(getter), setter_(setter), name_(name), description_(desc), icon_(icon) {}
    
private:
    Getter getter_;
    Setter setter_;
    
    QIcon icon_;
    QString name_;
    QString description_;
};


} //namespace remote

#endif

