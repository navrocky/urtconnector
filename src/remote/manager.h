
#ifndef URT_REMOTE_MANAGER_H
#define URT_REMOTE_MANAGER_H

#include <boost/function.hpp>

#include "remote.h"

namespace remote {



class manager {
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
    
    boost::shared_ptr<registrator> reg(const object& obj);

    inline const Objects& objects() const {return objects_;}
    
private:
    Objects objects_;
    std::map<QString, boost::function<Storage()> > factory_;
};

struct manager::object {
   
    object(){}
    
    object(const Getter& g, const Setter& s, const QString& name, const QString& desc);
        
    inline const QString& name() const { return name_; }
    inline const QString& description() const { return description_; }
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

