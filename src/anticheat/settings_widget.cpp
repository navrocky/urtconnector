#include <QFileDialog>

#include "ui_settings_widget.h"
#include "settings_widget.h"
#include "settings.h"

namespace anticheat
{

settings_widget::settings_widget(QWidget *parent)
: settings_base_widget(parent)
, ui_(new Ui::settings_widget)
{
    ui_->setupUi(this);

    connect(ui_->choose_local_folder_btn, SIGNAL(clicked()), SLOT(choose_local_folder()));
}

settings_widget::~settings_widget()
{
    delete ui_;
}

void settings_widget::update_contents()
{
    settings s;
    ui_->interval_spin->setValue(s.interval());
    ui_->quality_spin->setValue(s.quality());
    ui_->ftp_gb->setChecked(s.use_ftp());
    ui_->ftp_addr_edit->setText(s.ftp_address().address());
    ui_->login_edit->setText(s.ftp_login());
    ui_->password_edit->setText(s.ftp_password());
    ui_->ftp_folder_edit->setText(s.ftp_folder());
    ui_->local_file_gb->setChecked(s.use_local_folder());
    ui_->local_folder_edit->setText(s.local_folder());
}

void settings_widget::apply()
{
    settings s;
    s.set_interval(ui_->interval_spin->value());
    s.set_quality(ui_->quality_spin->value());
    s.set_use_ftp(ui_->ftp_gb->isChecked());
    s.set_ftp_address(server_id(ui_->ftp_addr_edit->text(), 21));
    s.set_ftp_login(ui_->login_edit->text());
    s.set_ftp_password(ui_->password_edit->text());
    s.set_ftp_folder(ui_->ftp_folder_edit->text());
    s.set_use_local_folder(ui_->local_file_gb->isChecked());
    s.set_local_folder(ui_->local_folder_edit->text());
}

void settings_widget::choose_local_folder()
{
    QString dir = QFileDialog::getExistingDirectory(this, 
        tr("Choose local folder to screenshots store"),
        ui_->local_folder_edit->text());
    if (dir.isEmpty())
        return;
    ui_->local_folder_edit->setText(dir);
}

}
