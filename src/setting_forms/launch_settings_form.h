
#ifndef URT_LAUNCH_SETTINGS_FORM_H
#define URT_LAUNCH_SETTINGS_FORM_H

#include <memory>

#include <preferences/src/preferences_widget.h>

class launch_settings_form: public preferences_widget {
Q_OBJECT
public:
    launch_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);

public slots:
    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();

private slots:
    void choose_binary();
    void choose_mumble_bin();
    void insert_file_path();
    void update_launch_string();
    void x_check();
    void int_changed();

private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    bool lock_change_;
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
