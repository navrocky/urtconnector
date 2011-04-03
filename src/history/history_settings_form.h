#ifndef URT_history_settings_form_H
#define URT_history_settings_form_H

#include <memory>
#include <preferences/src/preferences_widget.h>

class Ui_history_settings_form;

class history_settings_form : public preferences_widget
{
    Q_OBJECT
public:
    history_settings_form(QWidget* parent = 0, Qt::WindowFlags f = 0);
    virtual ~history_settings_form() {};

public slots:

    virtual void update_preferences();
    virtual void accept();
    virtual void reject();
    virtual void reset_defaults();

private slots:
    void int_changed();

private:
    std::auto_ptr<Ui_history_settings_form> ui_;
    bool changed_locked_;
};

#endif
