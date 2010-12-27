
#ifndef URT_LAUNCH_SETTINGS_FORM_H
#define URT_LAUNCH_SETTINGS_FORM_H

#include <memory>

#include <preferences/src/preferences_widget.h>

class launch_settings_form: public preferences_widget {
Q_OBJECT
public:
    launch_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~launch_settings_form();

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
    void choose_binary();
    void insert_file_path();
    void adv_text_changed(const QString&);
    void x_check();

private:
    void set_connections( bool b );
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;

};


class dialog_syncer: public QObject
{
    Q_OBJECT
public:
    dialog_syncer(){};
    ~dialog_syncer(){};

Q_SIGNALS:
    void accepted();
    void rejected();

public Q_SLOTS:
    void accept(){ emit accepted(); }
    void reject(){ emit rejected(); }
};


#endif
