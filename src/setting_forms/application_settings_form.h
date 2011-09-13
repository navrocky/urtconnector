#ifndef URT_APPLICATION_SETTINGS_FORM_H
#define URT_APPLICATION_SETTINGS_FORM_H

#include <memory>
#include <map>

#include <preferences/src/preferences_widget.h>

class application_settings_form : public preferences_widget
{
    Q_OBJECT
public:
    application_settings_form(QWidget* parent = 0);

private slots:
    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();
    void apply_style_sheet();
    void select_css_file();
    void select_sound_file();
    void play_sound_file();

    void int_changed();

private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
    bool lock_change_;
};

#endif



