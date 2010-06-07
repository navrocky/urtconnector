
#include <boost/thread.hpp>

#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>
#include <QMessageBox>
#include <QProgressDialog>

#include "options_dialog.h"
#include "launcher.h"
#include "app_options.h"
#include "launcher/tools.h"

options_dialog::options_dialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);

    connect( ui.select_bin_button, SIGNAL( clicked() ), SLOT( choose_binary() ));
    connect( ui.insertFileButton, SIGNAL( clicked() ), SLOT( insert_file_path() ));
    connect( ui.qstat_binary_choose_button, SIGNAL( clicked() ), SLOT( choose_qstat_binary() ));
    connect( ui.geoip_database_choose_button, SIGNAL( clicked() ), SLOT( choose_geoip_database()));
    connect( ui.advCmdEdit, SIGNAL(textChanged(const QString&)), SLOT(adv_text_changed(const QString&)));
    connect( ui.x_check_button, SIGNAL( clicked() ), SLOT( x_check() ));
    
    ui.adv_cmd_help_label->setText(tr(
        "<b>%bin%</b> - UrbanTerror binary path<br>"
        "<b>%name%</b> - player name<br>"
        "<b>%pwd%</b> - password<br>"
        "<b>%addr%</b> - hostname or ip and port<br>"
        "<b>%rcon%</b> - RCON password"
    ));
}

options_dialog::~options_dialog()
{
}

void options_dialog::set_opts(app_options_p value)
{
    opts_ = value;
    update_dialog();
}

void options_dialog::update_dialog()
{
    ui.binary_edit->setText( opts_->binary_path );
    ui.advCmdEdit->setText( opts_->adv_cmd_line );
    ui.advCmdBox->setChecked( opts_->use_adv_cmd_line );
    ui.qstat_binary_edit->setText( opts_->qstat_opts.qstat_path );
    ui.hide_mainwindow_check->setChecked(opts_->start_hidden);
    ui.geoip_database_edit->setText( opts_->geoip_database );
    ui.group_clipboard_watch->setChecked( opts_->looking_for_clip );
    ui.clip_regexp_edit->setText( opts_->lfc_regexp );
    ui.clip_host_spin->setValue( opts_->lfc_host );
    ui.clip_port_spin->setValue( opts_->lfc_port );
    ui.clip_password_spin->setValue( opts_->lfc_password );
    ui.separate_x_check->setChecked( opts_->separate_x );
}

void options_dialog::accept()
{
    QDialog::accept();
    opts_->binary_path = ui.binary_edit->text();
    opts_->adv_cmd_line = ui.advCmdEdit->text();
    opts_->use_adv_cmd_line = ui.advCmdBox->isChecked();
    opts_->qstat_opts.qstat_path = ui.qstat_binary_edit->text();
    opts_->start_hidden = ui.hide_mainwindow_check->isChecked();
    opts_->geoip_database = ui.geoip_database_edit->text();
    opts_->looking_for_clip = ui.group_clipboard_watch->isChecked();
    opts_->lfc_regexp = ui.clip_regexp_edit->text();
    opts_->lfc_host = ui.clip_host_spin->value();
    opts_->lfc_port = ui.clip_port_spin->value();
    opts_->lfc_password = ui.clip_password_spin->value();
    opts_->separate_x = ui.separate_x_check->isChecked();
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
    opts->use_adv_cmd_line = true;
    opts->adv_cmd_line = text;
    opts->binary_path = ui.binary_edit->text();

    launcher l(opts);
    l.set_server_id(server_id("server:12345"));
    l.set_user_name("New_URT_Player");
    l.set_rcon("rcon_pAsSwOrD");
    l.set_password("pAsSwOrD");
    l.set_referee("referee_pAsSwOrD");

    ui.adv_cmd_preview_edit->setText(l.launch_string());
}

namespace {
    
void test_thread( dialog_syncer& syncer)
{
    sleep(2);
    if ( try_x_start() )
        syncer.accept();
    else
        syncer.reject();
}

}

void options_dialog::x_check()
{
    QProgressDialog progress( tr("This may take about 30 seconds"), QString(), 0, 0, this );
    dialog_syncer syncer;
    connect( &syncer, SIGNAL( accepted() ), &progress, SLOT( accept() ) );
    connect( &syncer, SIGNAL( rejected() ), &progress, SLOT( reject() ) );

    boost::thread t( boost::bind( test_thread, boost::ref(syncer) ) );

    ui.separate_x_check->setChecked( progress.exec() );
    QMessageBox::information( this,
        tr( "X session autodetection" ),
        ( ui.separate_x_check->isChecked() )
            ? tr( "Another X session started succesfully" )
            : tr( "Another X session failed" )
    );
}

