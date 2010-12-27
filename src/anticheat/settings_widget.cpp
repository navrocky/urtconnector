#include <QFileDialog>

#include "ui_settings_widget.h"
#include "settings_widget.h"
#include "settings.h"

namespace anticheat
{

struct settings_widget::Pimpl{
    Ui::settings_widget ui;
};
    
settings_widget::settings_widget(QWidget *parent)
    : preferences_widget(parent, "Anticheat")
    , p_( new Pimpl )
{
    p_->ui.setupUi(this);

    set_icon( QIcon("icons:anticheat.png") );
    set_header( tr("Anticheat settings") );
    
    connect(p_->ui.choose_local_folder_btn, SIGNAL(clicked()), SLOT(choose_local_folder()));  
}

settings_widget::~settings_widget()
{}

void settings_widget::set_connections(bool b)
{
    if (b) {
        connect( p_->ui.interval_spin,      SIGNAL( valueChanged(int)),  SIGNAL(changed()) );
        connect( p_->ui.quality_spin,       SIGNAL( valueChanged(int)),  SIGNAL(changed()) );
        connect( p_->ui.ftp_gb,             SIGNAL( clicked(bool) ),     SIGNAL(changed()) );

        connect( p_->ui.ftp_addr_edit,      SIGNAL( textChanged(const QString&) ), SIGNAL(changed()) );
        connect( p_->ui.login_edit,         SIGNAL( textChanged(const QString&) ), SIGNAL(changed()) );
        connect( p_->ui.password_edit,      SIGNAL( textChanged(const QString&) ), SIGNAL(changed()) );
        connect( p_->ui.ftp_folder_edit,    SIGNAL( textChanged(const QString&) ), SIGNAL(changed()) );
        connect( p_->ui.only_md5_check,     SIGNAL( stateChanged(int) ), SIGNAL(changed()) );

        connect( p_->ui.local_file_gb,      SIGNAL( clicked(bool) ),     SIGNAL(changed()) );
        connect( p_->ui.local_folder_edit,  SIGNAL( textChanged(const QString&) ), SIGNAL(changed()) );
    }
    else{
        disconnect( p_->ui.interval_spin,      SIGNAL( valueChanged(int)),  this, SIGNAL(changed()) );
        disconnect( p_->ui.quality_spin,       SIGNAL( valueChanged(int)),  this,  SIGNAL(changed()) );
        disconnect( p_->ui.ftp_gb,             SIGNAL( clicked(bool) ),  this,     SIGNAL(changed()) );

        disconnect( p_->ui.ftp_addr_edit,      SIGNAL( textChanged(const QString&) ),  this, SIGNAL(changed()) );
        disconnect( p_->ui.login_edit,         SIGNAL( textChanged(const QString&) ),  this, SIGNAL(changed()) );
        disconnect( p_->ui.password_edit,      SIGNAL( textChanged(const QString&) ),  this, SIGNAL(changed()) );
        disconnect( p_->ui.ftp_folder_edit,    SIGNAL( textChanged(const QString&) ),  this, SIGNAL(changed()) );
        disconnect( p_->ui.only_md5_check,     SIGNAL( stateChanged(int) ), this, SIGNAL(changed()) );

        disconnect( p_->ui.local_file_gb,      SIGNAL( clicked(bool) ),  this,     SIGNAL(changed()) );
        disconnect( p_->ui.local_folder_edit,  SIGNAL( textChanged(const QString&) ),  this, SIGNAL(changed()) );
    }
        
}


void settings_widget::update_preferences()
{
    set_connections(false);

    settings s;
    
    p_->ui.interval_spin->setValue(s.interval());
    p_->ui.quality_spin->setValue(s.quality());
    
    p_->ui.ftp_gb->setChecked(s.use_ftp());
    p_->ui.ftp_addr_edit->setText(s.ftp_address().address());
    p_->ui.login_edit->setText(s.ftp_login());
    p_->ui.password_edit->setText(s.ftp_password());
    p_->ui.ftp_folder_edit->setText(s.ftp_folder());
    p_->ui.only_md5_check->setChecked(s.send_only_md5());
    
    p_->ui.local_file_gb->setChecked(s.use_local_folder());
    p_->ui.local_folder_edit->setText(s.local_folder());

    set_connections(true);
}


void settings_widget::accept()
{
    settings s;
        
    s.set_interval(p_->ui.interval_spin->value());
    s.set_quality(p_->ui.quality_spin->value());
    
    s.set_use_ftp(p_->ui.ftp_gb->isChecked());
    s.set_ftp_address(server_id(p_->ui.ftp_addr_edit->text(), 21));
    s.set_ftp_login(p_->ui.login_edit->text());
    s.set_ftp_password(p_->ui.password_edit->text());
    s.set_ftp_folder(p_->ui.ftp_folder_edit->text());
    s.set_send_only_md5(p_->ui.only_md5_check->isChecked());
    
    s.set_use_local_folder(p_->ui.local_file_gb->isChecked());
    s.set_local_folder(p_->ui.local_folder_edit->text());

    update_preferences();
}

void settings_widget::reject()
{
    update_preferences();
}

void settings_widget::reset_defaults()
{
    update_preferences();
}

void settings_widget::choose_local_folder()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        tr("Choose local folder to screenshots store"),
        p_->ui.local_folder_edit->text());
    if (dir.isEmpty())
        return;
    p_->ui.local_folder_edit->setText(dir);
}

}
