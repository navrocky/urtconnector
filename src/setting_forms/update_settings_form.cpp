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

    connect(p_->ui.qstat_binary_choose_button, SIGNAL(clicked()), SLOT(choose_qstat_binary()));
    connect(p_->ui.geoip_database_choose_button, SIGNAL(clicked()), SLOT(choose_geoip_database()));

    connect(p_->ui.center_current_row_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.clear_offline_check, SIGNAL(stateChanged(int)), this, SLOT(int_changed()));
    connect(p_->ui.qstat_binary_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
    connect(p_->ui.qstat_master_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
    connect(p_->ui.geoip_database_edit, SIGNAL(textChanged(const QString &)), this, SLOT(int_changed()));
}


void update_settings_form::int_changed()
{
    if (!lock_change_)
        emit changed();
}

void update_settings_form::update_preferences()
{
    scoped_value_change<bool> s(lock_change_, true, false);

    app_settings as;
    qstat_options qs;

    p_->ui.center_current_row_check->setChecked(as.center_current_row());
    p_->ui.clear_offline_check->setChecked(as.clear_offline());
    p_->ui.qstat_binary_edit->setText(qs.qstat_path());
    p_->ui.qstat_master_edit->setText(qs.master_server());
    p_->ui.geoip_database_edit->setText(as.geoip_database());
}

void update_settings_form::accept()
{
    app_settings as;
    qstat_options qs;

    as.set_center_current_row(p_->ui.center_current_row_check->isChecked());
    as.set_clear_offline(p_->ui.clear_offline_check->isChecked());
    as.set_geoip_database(p_->ui.geoip_database_edit->text());
    qs.qstat_path_set(p_->ui.qstat_binary_edit->text());
    qs.master_server_set(p_->ui.qstat_master_edit->text());
}

void update_settings_form::reject()
{
    update_preferences();
}

void update_settings_form::reset_defaults()
{
    update_preferences();
}

void update_settings_form::choose_qstat_binary()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("QStat binary"), "",
                                                     tr("Executables (*);;All Files (*)"));
    if (file_name.isEmpty()) return;

    p_->ui.qstat_binary_edit->setText(file_name);
}

void update_settings_form::choose_geoip_database()
{
    QString file_name = QFileDialog::getOpenFileName(this,
                                                     tr("GeoIP database"), "",
                                                     tr("DataBase (*.dat);;All Files (*)"));
    if (file_name.isEmpty()) return;

    p_->ui.geoip_database_edit->setText(file_name);
}
