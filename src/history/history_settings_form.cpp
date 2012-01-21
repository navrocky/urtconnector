#include "history_settings_form.h"

#include <QSpinBox>
#include <QRadioButton>

#include <common/scoped_tools.h>
#include "../app_options.h"

#include "ui_history_settings_form.h"

history_settings_form::history_settings_form(QWidget* parent, Qt::WindowFlags f)
: preferences_widget(parent, tr("History"), f)
, ui_( new Ui_history_settings_form )
, changed_locked_(false)
{
    ui_->setupUi(this);

    set_icon(QIcon("icons:history.png"));
    set_header(tr("History settings"));

    connect(ui_->number_in_history_spin, SIGNAL(valueChanged(int)), SLOT(int_changed()));
    connect(ui_->days_history_spin,      SIGNAL(valueChanged(int)), SLOT(int_changed()));
    connect(ui_->group_linear_radio,     SIGNAL(toggled(bool)), SLOT(int_changed()));
    connect(ui_->group_by_server_radio,  SIGNAL(toggled(bool)), SLOT(int_changed()));
}

void history_settings_form::update_preferences()
{
    scoped_value_change<bool> s(changed_locked_, true, false);

    app_settings as;
    ui_->number_in_history_spin->setValue(as.number_in_history());
    ui_->days_history_spin->setValue(as.days_in_history());

    if (as.history_grouping())
        ui_->group_by_server_radio->setChecked(true);
    else
        ui_->group_linear_radio->setChecked(true);
}


void history_settings_form::accept()
{
    app_settings as;
    as.number_in_history_set(ui_->number_in_history_spin->value());
    as.days_in_history_set(ui_->days_history_spin->value());
    as.history_grouping_set(ui_->group_by_server_radio->isChecked());
    update_preferences();
}

void history_settings_form::reject()
{
    update_preferences();
}

void history_settings_form::reset_defaults()
{
    ui_->number_in_history_spin->setValue(900);
    ui_->days_history_spin->setValue(90);
    ui_->group_by_server_radio->setChecked(true);
    accept();
}

void history_settings_form::int_changed()
{
    if (!changed_locked_)
        emit changed();
}
