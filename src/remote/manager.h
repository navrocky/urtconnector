
#ifndef URT_REMOTE_MANAGER_H
#define URT_REMOTE_MANAGER_H

#include <boost/function.hpp>

#include "remote.h"

namespace remote {



class manager: public QObject {
public:
    struct subject;
//     class registrator;
    
    typedef boost::function<remote::object ()> Getter;
    typedef boost::function<void(const remote::object&)> Setter;
    
    typedef boost::function<bool (const object&, const object&)> ObjectCompare;
    
   
    typedef boost::shared_ptr<const subject> Subject;
    
    
    
    typedef std::map<Subject, std::list<service::Storage> > Subjects;

    
    manager();


    const std::list<Service>& services() const;

    service::Storage create(const Service&);

  
    

    Subject attach(const QString& name, const Getter& g, const Setter& s, const QString& desc) {}
    void detach(const Subject&){}


    //TODO убрать
//     boost::shared_ptr<registrator> reg(const object& obj);

//     inline const Objects& objects() const {return objects_;}

    void bind(const Subject& subject, const service::Storage& storage);

    void sync(const Subject& obj);
    
    void sync_impl();


public Q_SLOTS:
    void loaded(const remote::object& obj);
    
private:
    Subjects subjects_;

    std::list<Service> services_;

    struct queued {
        queued(
            const Subject& subj,
            const std::list<service::Storage>& st,
            const remote::object& obj)
        : subject(subj), storages(st), object(obj), entries(object.entries())
        {}
        
        Subject subject;
        std::list<service::Storage> storages;
        remote::object object;
        remote::object::Entries entries;
    };
    
    std::list<queued> sync_queue_;
    
/*    
    std::map<boost::sha, boost::function<Storage()> > factory_;*/
};

struct manager::subject {
   
    subject(){}
    
    subject(const Getter& g, const Setter& s, const QString& name, const QString& desc);
        
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

