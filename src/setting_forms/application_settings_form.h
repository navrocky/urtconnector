
#ifndef URT_APPLICATION_SETTINGS_FORM_H
#define URT_APPLICATION_SETTINGS_FORM_H



#include <memory>
#include <map>

#include <preferences/src/preferences_widget.h>

class application_settings_form: public preferences_widget {
Q_OBJECT
public:
    application_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~application_settings_form();

private Q_SLOTS:

    ///Этот слот вызывается перед показом виджета и загрузка всех настроек должна поисходить здесь
    virtual void update_preferences();

    ///Этот слот вызывается когда изменения приняты
    virtual void accept();

    ///Этот слот вызывается когда изменения отклонены
    virtual void reject();

    ///Этот слот вызывается чтоб узнановить значения по умолчанию
    virtual void reset_defaults();

private Q_SLOTS:
    void choose_qstat_binary();
    void choose_geoip_database();

private:
    void set_connections( bool b );
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;

};


#endif



