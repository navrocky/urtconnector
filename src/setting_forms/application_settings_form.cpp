
#include <QFileDialog>

#include "app_options.h"
#include "qstat_options.h"

#include "ui_application_settings_form.h"
#include "application_settings_form.h"


struct application_settings_form::Pimpl{
    Ui_application_settings_form ui;
};

application_settings_form::application_settings_form(QWidget* parent, Qt::WindowFlags f)
    : preferences_widget(parent, "Application")
    , p_( new Pimpl )
{
    p_->ui.setupUi(this);

    set_icon( QIcon("icons:configure.png") );
    set_header( tr("Main application settings") );
    
    connect( p_->ui.qstat_binary_choose_button,     SIGNAL( clicked() ), SLOT( choose_qstat_binary() ));
    connect( p_->ui.geoip_database_choose_button,   SIGNAL( clicked() ), SLOT( choose_geoip_database()));   
}

application_settings_form::~application_settings_form()
{}

void application_settings_form::set_connections(bool b)
{
    if(b) {
        connect( p_->ui.hide_mainwindow_check,    SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );

        connect( p_->ui.center_current_row_check, SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );

        connect( p_->ui.holiday_check,            SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );

        connect( p_->ui.qstat_binary_edit,        SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        connect( p_->ui.qstat_master_edit,        SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        connect( p_->ui.geoip_database_edit,      SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        connect( p_->ui.group_clipboard_watch,    SIGNAL( clicked(bool) ),              this, SIGNAL( changed() ) );
        connect( p_->ui.clip_regexp_edit,         SIGNAL( textChanged(const QString&)), this, SIGNAL( changed() ) );

        connect( p_->ui.clip_host_spin,           SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
        connect( p_->ui.clip_port_spin,           SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
        connect( p_->ui.clip_password_spin,       SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );

        connect( p_->ui.group_keep_history,       SIGNAL( clicked(bool) ),              this, SIGNAL( changed() ) );
        connect( p_->ui.number_in_history_spin,   SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
    }
    else {
        disconnect( p_->ui.hide_mainwindow_check,    SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );

        disconnect( p_->ui.center_current_row_check, SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );
        
        disconnect( p_->ui.holiday_check,            SIGNAL(stateChanged(int)),            this, SIGNAL( changed() ) );

        disconnect( p_->ui.qstat_binary_edit,        SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        disconnect( p_->ui.qstat_master_edit,        SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        disconnect( p_->ui.geoip_database_edit,      SIGNAL(textChanged(const QString &)), this, SIGNAL( changed() ) );

        disconnect( p_->ui.group_clipboard_watch,    SIGNAL( clicked(bool) ),              this, SIGNAL( changed() ) );
        disconnect( p_->ui.clip_regexp_edit,         SIGNAL( textChanged(const QString&)), this, SIGNAL( changed() ) );

        disconnect( p_->ui.clip_host_spin,           SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
        disconnect( p_->ui.clip_port_spin,           SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
        disconnect( p_->ui.clip_password_spin,       SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );

        disconnect( p_->ui.group_keep_history,       SIGNAL( clicked(bool) ),              this, SIGNAL( changed() ) );
        disconnect( p_->ui.number_in_history_spin,   SIGNAL( valueChanged(int) ),          this, SIGNAL( changed() ) );
    }
}


void application_settings_form::update_preferences()
{
    set_connections(false);

    app_settings    as;
    qstat_settings  qs;
    clip_settings   cs;
    
    p_->ui.hide_mainwindow_check->setChecked( as.start_hidden() );
    p_->ui.center_current_row_check->setChecked( as.center_current_row() );
    p_->ui.holiday_check->setChecked( as.use_holiday_mode() );

    p_->ui.qstat_binary_edit->setText( qs.qstat_path() );
    p_->ui.qstat_master_edit->setText( qs.master_server() );

    p_->ui.geoip_database_edit->setText( as.geoip_database() );
    
    p_->ui.group_clipboard_watch->setChecked( cs.watching() );
    p_->ui.clip_regexp_edit->setText( cs.regexp() );
    p_->ui.clip_host_spin->setValue( cs.host() );
    p_->ui.clip_port_spin->setValue( cs.port() );
    p_->ui.clip_password_spin->setValue( cs.password() );

    p_->ui.group_keep_history->setChecked( as.keep_history() );
    p_->ui.number_in_history_spin->setValue( as.number_in_history() );

    set_connections(true);
}


void application_settings_form::accept()
{
    app_settings    as;
    qstat_settings  qs;
    clip_settings   cs;
    
    as.set_start_hidden( p_->ui.hide_mainwindow_check->isChecked() );
    as.set_center_current_row( p_->ui.center_current_row_check->isChecked() );
    as.set_holiday_mode( p_->ui.holiday_check->isChecked() );

    qs.set_qstat_path( p_->ui.qstat_binary_edit->text() );
    qs.set_master_server( p_->ui.qstat_master_edit->text() );
    
    as.set_geoip_database( p_->ui.geoip_database_edit->text() );

    cs.set_watching( p_->ui.group_clipboard_watch->isChecked() );
    cs.set_regexp( p_->ui.clip_regexp_edit->text() );
    cs.set_host( p_->ui.clip_host_spin->value() );
    cs.set_port( p_->ui.clip_port_spin->value() );
    cs.set_password( p_->ui.clip_password_spin->value() );

    as.set_keep_history( p_->ui.group_keep_history->isChecked() );
    as.set_number_in_history( p_->ui.number_in_history_spin->value() );
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

void application_settings_form::choose_qstat_binary()
{
    QString file_name = QFileDialog::getOpenFileName(this,
      tr("QStat binary"), "",
      tr("Executables (*);;All Files (*)"));
    if (file_name.isEmpty()) return;

    p_->ui.qstat_binary_edit->setText(file_name);
}

void application_settings_form::choose_geoip_database()
{
    QString file_name = QFileDialog::getOpenFileName(this,
      tr("GeoIP database"), "",
      tr("DataBase (*.dat);;All Files (*)"));
    if (file_name.isEmpty()) return;

    p_->ui.geoip_database_edit->setText(file_name);
}



