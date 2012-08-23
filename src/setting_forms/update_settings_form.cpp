#include "update_settings_form.h"

#include <QFileDialog>

#include <common/scoped_tools.h>
#include <common/qstat_options.h>
#include "app_options.h"

#include "ui_update_settings_form.h"

struct update_settings_form::Pimpl
{
    Ui_update_settings_form ui;
};

update_settings_form::update_settings_form(QWidget* parent)
: preferences_widget(parent, tr("Updating"))
, p_(new Pimpl)
, lock_change_(false)
{
    p_->ui.setupUi(this);

    set_icon(QIcon("icons:view-refresh.png"));
    set_header(tr("Server list updating settings"));

    connect(p_->ui.geoip_database_choose_button, SIGNAL(clicked()), SLOT(choose_geoip_database()));
    connect(p_->ui.center_current_row_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clear_offline_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.qstat_master_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
    connect(p_->ui.geoip_database_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
    connect(p_->ui.maxsim_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.retry_spin, SIGNAL(valueChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.timeout_spin, SIGNAL(valueChanged(double)), this, SLOT(int_changed()));
    connect(p_->ui.series_timeout_spin, SIGNAL(valueChanged(double)), this, SLOT(int_changed()));
}


void update_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void update_settings_form::update_preferences()
{
    SCOPE_COCK_FLAG(lock_change_);

    app_settings as;
    p_->ui.center_current_row_check->setChecked(as.center_current_row());
    p_->ui.clear_offline_check->setChecked(as.clear_offline());
    p_->ui.geoip_database_edit->setText(as.geoip_database());

    qstat_options qs;
    p_->ui.qstat_master_edit->setText(qs.master_server());
    p_->ui.maxsim_spin->setValue(qs.max_sim_queries());
    p_->ui.retry_spin->setValue(qs.retry_number());
    p_->ui.timeout_spin->setValue(qs.timeout() / 1000.0);
    p_->ui.series_timeout_spin->setValue(qs.series_timeout() / 1000.0);
}

void update_settings_form::accept()
{
    app_settings as;
    as.center_current_row_set(p_->ui.center_current_row_check->isChecked());
    as.clear_offline_set(p_->ui.clear_offline_check->isChecked());
    as.geoip_database_set(p_->ui.geoip_database_edit->text());

    qstat_options qs;
    qs.master_server_set(p_->ui.qstat_master_edit->text());
    qs.max_sim_queries_set(p_->ui.maxsim_spin->value());
    qs.retry_number_set(p_->ui.retry_spin->value());
    qs.timeout_set(qRound(p_->ui.timeout_spin->value() * 1000.0));
    qs.series_timeout_set(qRound(p_->ui.series_timeout_spin->value() * 1000.0));
}

void update_settings_form::reject()
{
    update_preferences();
}

void update_settings_form::reset_defaults()
{
    qstat_options qo;
    qo.master_server_reset();
    qo.max_sim_queries_reset();
    qo.retry_number_reset();
    qo.timeout_reset();
    qo.series_timeout_reset();

    update_preferences();
}

void update_settings_form::choose_geoip_database()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("GeoIP database"), "",
                                                     tr("DataBase (*.dat);;All Files (*)"));
    if (file_name.isEmpty()) return;

    p_->ui.geoip_database_edit->setText(file_name);
}
