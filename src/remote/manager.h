
#ifndef URT_REMOTE_MANAGER_H
#define URT_REMOTE_MANAGER_H

#include <boost/function.hpp>

#include "remote.h"

namespace remote {



class manager: public QObject {
public:
    struct object;
    class registrator;
    
    typedef boost::function<remote::object ()> Getter;
    typedef boost::function<void(const remote::object&)> Setter;
    
    typedef boost::function<bool (const object&, const object&)> ObjectCompare;
    
    typedef boost::shared_ptr<storage> Storage;
    typedef std::list<Storage> Storages;
    
    typedef boost::shared_ptr<object> Object;
    
    typedef std::map<Object, Storages> Objects;

    
    manager();


    const std::list<Service>& services() const;


    boost::shared_ptr<storage> create(Service);


    
    

    shared_ptr<object> attach(const QString& name, const Getter& g, const Setter& s, const QString& desc) {}
    void detach(shared_ptr<object>){}


    //TODO убрать
    boost::shared_ptr<registrator> reg(const object& obj);

    inline const Objects& objects() const {return objects_;}

    void bind(const Object& obj, const boost::shared_ptr<storage>& );

    void sync(const Object& obj);


public Q_SLOTS:
    void loaded(const remote::object& obj);
    
private:
    Objects objects_;

    std::list<Service> services_;

    std::map<Object, Storages> sync_queue_;
    
/*    
    std::map<boost::sha, boost::function<Storage()> > factory_;*/
};

struct manager::object {
   
    object(){}
    
    object(const Getter& g, const Setter& s, const QString& name, const QString& desc);
        
    inline const QString& name() const { return name_; }
    inline const QString& description() const { return description_; }

    remote::object get() const { return getter_(); }
    void put(const remote::object& obj) { setter_(obj); }
    
private:
    Getter getter_;
    Setter setter_;
    
    QString name_;
    QString description_;
};


struct manager::registrator {
    
    template<typename Eraser>
    registrator(const Eraser& eraser) : eraser_(eraser) {}
   
    ~registrator() {eraser_();}
    
private:
    boost::function<void()> eraser_;
};
    




} //namespace remote

#endif

