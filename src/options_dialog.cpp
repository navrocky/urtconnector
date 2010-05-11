#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>

#include "options_dialog.h"
#include "launcher.h"
#include "app_options.h"

options_dialog::options_dialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);

    connect( ui.selectBinButton, SIGNAL( clicked() ), SLOT( choose_binary() ));
    connect( ui.insertFileButton, SIGNAL( clicked() ), SLOT( insert_file_path() ));
    connect( ui.qstat_binary_choose_button, SIGNAL( clicked() ), SLOT( choose_qstat_binary() ));
    connect( ui.geoip_database_choose_button, SIGNAL( clicked() ), SLOT( choose_geoip_database()));
    connect( ui.advCmdEdit, SIGNAL(textChanged(const QString&)), SLOT(adv_text_changed(const QString&)));
}

options_dialog::~options_dialog()
{
    opts_->sync();
}

void options_dialog::set_opts(app_options_p value)
{
    opts_ = value;
    update_dialog();
}

void options_dialog::update_dialog()
{
    ui.hide_mainwindow_check->setChecked(opts_->main.value<bool>("start_hidden"));
    ui.geoip_database_edit->setText(opts_->main.value<QString>("geoip_database"));
    ui.looking_for_clip->setChecked(opts_->main.value<bool>("looking_for_clip"));
    
    ui.binary_edit->setText(opts_->launch.value<QString>("binary_path"));
    ui.advCmdEdit->setText(opts_->launch.value<QString>("adv_cmd_line"));
    ui.advCmdBox->setChecked(opts_->launch.value<bool>("use_adv_cmd_line"));
    
    ui.qstat_binary_edit->setText( opts_->qstat.value<QString>("path") );
}

void options_dialog::accept()
{
    QDialog::accept();
    opts_->main.set_value("start_hidden", ui.hide_mainwindow_check->isChecked() );
    opts_->main.set_value("geoip_database", ui.geoip_database_edit->text() );
    opts_->main.set_value("looking_for_clip", ui.looking_for_clip->isChecked() );
    
    opts_->launch.set_value("binary_path", ui.binary_edit->text());
    opts_->launch.set_value("adv_cmd_line", ui.advCmdEdit->text() );
    opts_->launch.set_value("use_adv_cmd_line", ui.advCmdBox->isChecked() );

    opts_->qstat.set_value("path", ui.qstat_binary_edit->text() );
}

void options_dialog::choose_binary()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Urban Terror executable"), "",
      tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.binary_edit->setText(fileName);
}

void options_dialog::choose_geoip_database()
{
    QString file_name = QFileDialog::getOpenFileName(this,
      tr("GeoIP database"), "",
      tr("DataBase (*.dat);;All Files (*)"));
    if (file_name.isEmpty()) return;

    ui.geoip_database_edit->setText(file_name);
}

void options_dialog::choose_qstat_binary()
{
    QString file_name = QFileDialog::getOpenFileName(this,
      tr("QStat binary"), "",
      tr("Executables (*);;All Files (*)"));
    if (file_name.isEmpty()) return;

    ui.qstat_binary_edit->setText(file_name);
}

void options_dialog::insert_file_path()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Select file to insert"), "",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.advCmdEdit->insert(fileName);
}

void options_dialog::adv_text_changed(const QString& text)
{
    app_options_p opts(new app_options(*opts_));
    opts->launch.set_value("adv_cmd_line", text);
    opts->launch.set_value("binary_path", ui.binary_edit->text());

    launcher l(opts->launch);
    l.set_server_id(server_id("server:12345"));
    l.set_user_name("New_URT_Player");
    l.set_rcon("rcon_pAsSwOrD");
    l.set_password("pAsSwOrD");
    l.set_referee("referee_pAsSwOrD");

    ui.adv_cmd_preview_edit->setText(l.launch_string());
}
