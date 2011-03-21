#ifndef UPDATE_SETTINGS_FORM_H
#define UPDATE_SETTINGS_FORM_H

#include <memory>
#include <map>

#include <preferences/src/preferences_widget.h>

class update_settings_form: public preferences_widget {
    Q_OBJECT
public:
    update_settings_form(QWidget* parent = 0);

private slots:
    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();

    void int_changed();

    void choose_qstat_binary();
    void choose_geoip_database();

private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    bool lock_change_;
};

#endif



