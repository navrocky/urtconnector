#ifndef SYNC_SETTINGS_FORM_H
#define SYNC_SETTINGS_FORM_H

#include <memory>
#include <map>

#include <remote/manager.h>
#include <preferences/src/preferences_widget.h>

class QListWidgetItem;
class QTreeWidgetItem;

class sync_settings_form: public preferences_widget {
    Q_OBJECT
public:
    sync_settings_form(boost::shared_ptr<remote::syncro_manager> sync_man, QWidget* parent = 0);
    virtual ~sync_settings_form(){}

private slots:
    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();
    
    void int_changed();

    void current_srv_changed ( QListWidgetItem * current, QListWidgetItem * previous );
    void current_st_changed ( QListWidgetItem * current, QListWidgetItem * previous );
    
    void storage_changed(const remote::syncro_manager::Storage& current, const remote::syncro_manager::Storage& previous);
    void object_changed(const remote::syncro_manager::Object& current);
	void object_attached(const remote::syncro_manager::Object& current);
	void object_detached(const remote::syncro_manager::Object& current);
    
    void do_import();
    void do_export();
    void do_sync();    
    

    //  service functions
    void create();
    
    //  storage functions
    void erase();
    void edit();
    void unbind_storage();
    
    // object functions
    void unbind();
    void bind();
    
    // All-object functions
    void unbind_object();

private:
    remote::syncro_manager::Service srv_current() const;
    remote::syncro_manager::Storage st_current() const;
    remote::syncro_manager::Object obj_current() const;
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    bool lock_change_;
};



class sync_settings_form2: public preferences_widget {
    Q_OBJECT
public:
    sync_settings_form2(boost::shared_ptr<remote::syncro_manager> sync_man, QWidget* parent = 0);
    virtual ~sync_settings_form2(){}
    
    
private slots:
    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();
    
    void current_storage_changed (QTreeWidgetItem * current, QTreeWidgetItem * previous);
    void current_object_changed (QListWidgetItem * current, QListWidgetItem * previous);

    void update();
    
    void create();
    void edit();
    void destroy();    
    
    void bind();
    void unbind();
    void unbindall();
    
    void do_import();
    void do_export();
    void do_sync();    

private:
    template <typename T>
    T current() const;
    remote::syncro_manager::Object current_object() const;
//     remote::syncro_manager::Object obj_current() const;
   
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    bool lock_change_;
};







#endif



