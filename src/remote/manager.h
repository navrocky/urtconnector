
#ifndef URT_REMOTE_MANAGER_H
#define URT_REMOTE_MANAGER_H

#include <boost/function.hpp>

#include "remote.h"

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
    typedef boost::shared_ptr<const service> Service;
    
    /*! Type of access key to existing storage */
    typedef boost::shared_ptr<const storage> Storage;
    


    syncro_manager();


    /*! List of registered services */
    const std::list<Service>& services() const;

    /*! list of attached objects */
    std::list<Object> objects() const;

    /*! list of created storages */
    std::list<Storage> storages() const;
    
    /*! create new instance of storage provided by service */
    service::Storage create(const Service&);

    /*! attach callbacks to manager, and get object to interact
     * \p getter - callback to get data when synchronize needed
     * \p setter - callback to set data after synchronization
     * \p name - name must be uniqe within one manager \example "bookmarks"
     * \p desc - description
     */    
    Object attach(const QString& name, const Getter& g, const Setter& s, const QString& desc);
    
    /*! detacj object from manager */
    void detach(const Object&);

    void bind(const Object& subject, const Storage& storage);

    void sync(const Object& obj);

public Q_SLOTS:
    void loaded(const remote::group& obj);
    void error(const QString& err);
    void finished();

   
private:

    typedef std::set<Storage> Storages;
    typedef std::map<Object, Storages> Objects;
    
    
    
private:
    std::list<Service> services_;
    Storages storages_;
    Objects objects_;

    

    struct sync_task {
        
        sync_task(
            const Object& obj,
            const Storages& st,
            const remote::group& gr)
        : object(obj), storages(st), group(gr), entries(gr.entries())
        {}

        bool operator<(const sync_task& other) const { return object < other.object; }
        bool operator==(const sync_task& other) const { return object == other.object; }
        
        Storages::iterator current_storage() { return storages.begin(); }
        
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
    }
    
/*    
    std::map<boost::sha, boost::function<Storage()> > factory_;*/
};

/*! \brief This object represents one element of syncronization
 * For example: "bookmarks" or "profile"
 */
class syncro_manager::object {

friend class syncro_manager;

    object(const Getter& getter, const Setter& setter, const QString& name, const QString& desc)
        : getter_(getter), setter_(setter), name_(name), description_(desc)
    {}
public:

    inline const QString& name() const { return name_; }
    inline const QString& description() const { return description_; }

    virtual remote::group get() const { return getter_(); };
    virtual void put(const remote::group& gr) { setter_(gr); };
    
private:
    Getter getter_;
    Setter setter_;
    
    QString name_;
    QString description_;
};


// struct manager::registrator {
//     
//     template<typename Eraser>
//     registrator(const Eraser& eraser) : eraser_(eraser) {}
//    
//     ~registrator() {eraser_();}
//     
// private:
//     boost::function<void()> eraser_;
// };
//     




} //namespace remote

#endif

