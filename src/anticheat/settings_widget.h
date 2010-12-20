#pragma once

#include <memory>

#include <preferences/src/preferences_widget.h>



namespace anticheat
{

class settings_widget : public preferences_widget
{
    Q_OBJECT
public:
    settings_widget(QWidget *parent = 0);
    ~settings_widget();

private Q_SLOTS:

    ///Этот слот вызывается перед показом виджета и загрузка всех настроек должна поисходить здесь
    virtual void update_preferences();

    ///Этот слот вызывается когда изменения приняты
    virtual void accept();

    ///Этот слот вызывается когда изменения отклонены
    virtual void reject();

    ///Этот слот вызывается чтоб узнановить значения по умолчанию
    virtual void reset_defaults();
    
private slots:
    void choose_local_folder();

    void set_connections( bool b );
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

}

