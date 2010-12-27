
#ifndef URT_RCON_SETTINGS_FORM_H
#define URT_RCON_SETTINGS_FORM_H

#include <memory>
#include <map>

#include <preferences/src/preferences_widget.h>

class rcon_settings_form: public preferences_widget {
Q_OBJECT
public:
    rcon_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~rcon_settings_form();

private Q_SLOTS:

    ///Этот слот вызывается перед показом виджета и загрузка всех настроек должна поисходить здесь
    virtual void update_preferences();

    ///Этот слот вызывается когда изменения приняты
    virtual void accept();

    ///Этот слот вызывается когда изменения отклонены
    virtual void reject();

    ///Этот слот вызывается чтоб узнановить значения по умолчанию
    virtual void reset_defaults();
    
    
    void custom_checked( bool b );
    void color_clicked();
//     void reset_to_defaults();
private:
//     void init();    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;

};


#endif
