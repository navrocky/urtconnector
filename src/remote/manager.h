
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
    
//     typedef boost::function<bool (const group&, const group&)> ObjectCompare;
    
    /*! Access key to attached object */
    typedef boost::shared_ptr<object> Object;


    syncro_manager();


    /*! List of registered services */
    const std::list<Service>& services() const;

    /*! list of attached objects */
    std::list<Object> objects() const;

    /*! create new instance of storage provided by service */
    service::Storage create(const Service&);

    /*! attach callbacks to manager, and get object to interact */
    Object attach(const QString& name, const Getter& g, const Setter& s, const QString& desc);
    
    /*! detacj object from manager */
    void detach(const Object&);

    void bind(const Object& subject, const service::Storage& storage);

    void sync(const Object& obj);
    
    void sync_impl();


public Q_SLOTS:
    void loaded(const remote::group& obj);
    void error(const QString& err);
    void finished();

   
private:
    
    /*! Object for syncing: "bookmarks", "config", etc. */
    struct complete_object;
    typedef boost::shared_ptr<complete_object> CompleteObject;
    typedef std::list<service::Storage> Storages;
    
    typedef std::map<CompleteObject, Storages> Objects;
    
    
    
private:
    
    inline CompleteObject promote(const Object& obj) const;
    
    
    Objects objects_;

    std::list<Service> services_;

    struct task {
        task(
            const Object& obj,
            const std::list<service::Storage>& st,
            const remote::group& gr)
        : object(obj), storages(st), group(gr), entries(gr.entries())
        {}

        bool operator<(const task& other) const { return object < other.object; }
        
        Object object;
        std::list<service::Storage> storages;
        remote::group group;
        remote::group::Entries entries;
    };
    
    std::set<task> tasks_;
    
/*    
    std::map<boost::sha, boost::function<Storage()> > factory_;*/
};

/*! \brief This object represents one element of syncronization
 * For example: "bookmarks" or "profile"
 */
struct syncro_manager::object {
    object(){}

    /*! Construct complete subject for registering at manager
     * \p getter - callback to get data when synchronize needed
     * \p setter - callback to set data after synchronization
     * \p name - name must be uniqe within one manager \example "bookmarks"
     * \p desc - description
     */
    object(const Getter& getter, const Setter& setter, const QString& name, const QString& desc);
        
    virtual inline const QString& name() const = 0;
    virtual inline const QString& description() const = 0;

    virtual remote::group get() const = 0;
    virtual void put(const remote::group& gr) = 0;
    

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

