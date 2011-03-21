
#include <QFileDialog>

#include <common/scoped_tools.h>
#include "app_options.h"
#include "qstat_options.h"

#include "ui_application_settings_form.h"
#include "application_settings_form.h"

struct application_settings_form::Pimpl
{
    Ui_application_settings_form ui;
};

application_settings_form::application_settings_form(QWidget* parent)
: preferences_widget(parent, tr("Application"))
, p_(new Pimpl)
, lock_change_(false)
{
    p_->ui.setupUi(this);

    set_icon(QIcon("icons:configure.png"));
    set_header(tr("Application settings"));

    connect(p_->ui.hide_mainwindow_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.holiday_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.group_clipboard_watch, SIGNAL(clicked(bool)), this, SLOT(int_changed()));
    connect(p_->ui.clip_regexp_edit, SIGNAL(textChanged(const QString&)), this, SLOT(int_changed()));
    connect(p_->ui.clip_host_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_port_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clip_password_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
}

void application_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void application_settings_form::update_preferences()
{
    scoped_value_change<bool> s(lock_change_, true, false);

    app_settings as;
    clip_settings cs;
    p_->ui.hide_mainwindow_check->setChecked(as.start_hidden());
    p_->ui.holiday_check->setChecked(as.use_holiday_mode());

    p_->ui.group_clipboard_watch->setChecked(cs.watching());
    p_->ui.clip_regexp_edit->setText(cs.regexp());
    p_->ui.clip_host_spin->setValue(cs.host());
    p_->ui.clip_port_spin->setValue(cs.port());
    p_->ui.clip_password_spin->setValue(cs.password());
}

void application_settings_form::accept()
{
    app_settings as;
    clip_settings cs;

    as.set_start_hidden(p_->ui.hide_mainwindow_check->isChecked());
    as.set_holiday_mode(p_->ui.holiday_check->isChecked());

    cs.set_watching(p_->ui.group_clipboard_watch->isChecked());
    cs.set_regexp(p_->ui.clip_regexp_edit->text());
    cs.set_host(p_->ui.clip_host_spin->value());
    cs.set_port(p_->ui.clip_port_spin->value());
    cs.set_password(p_->ui.clip_password_spin->value());
}

void application_settings_form::reject()
{
    update_preferences();
}

void application_settings_form::reset_defaults()
{
    clip_settings().reset_regexp();

    update_preferences();
}
