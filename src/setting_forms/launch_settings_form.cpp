
#include <iostream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>

#include "app_options.h"
#include <launcher/launcher.h>
#include <launcher/tools.h>

#include "ui_launch_settings_form.h"
#include "launch_settings_form.h"


struct launch_settings_form::Pimpl{
    Ui_launch_settings_form ui;
};

launch_settings_form::launch_settings_form(QWidget* parent, Qt::WindowFlags f)
    : preferences_widget(parent, "Launch")
    , p_( new Pimpl )
{
    p_->ui.setupUi(this);

    set_icon( QIcon(":/images/icons/logo.png") );
    set_header( tr("UrbanTerror launch settings") );
    
    connect( p_->ui.select_bin_button,  SIGNAL( clicked() ),                    this, SLOT( choose_binary() ));
    connect( p_->ui.insertFileButton,   SIGNAL( clicked() ),                    this, SLOT( insert_file_path() ));
    connect( p_->ui.advCmdEdit,         SIGNAL( textChanged(const QString&)),   this, SLOT( adv_text_changed(const QString&) ) );
    connect( p_->ui.x_check_button,     SIGNAL( clicked() ),                    this, SLOT( x_check() ));

    p_->ui.adv_cmd_help_label->setText(tr(
        "<b>%bin%</b> - UrbanTerror binary path<br>"
        "<b>%name%</b> - player name<br>"
        "<b>%pwd%</b> - password<br>"
        "<b>%addr%</b> - hostname or ip and port<br>"
        "<b>%rcon%</b> - RCON password"
    ));
    
#ifndef Q_OS_UNIX
    p_->ui.separate_x_check->setVisible(false);
    p_->ui.x_check_button->setVisible(false);
#endif
    
}

launch_settings_form::~launch_settings_form()
{}

void launch_settings_form::set_connections(bool b)
{
    if (b) {
        connect( p_->ui.binary_edit,        SIGNAL( textChanged(const QString &) ), this, SIGNAL( changed() ) );
    
        connect( p_->ui.advCmdBox,          SIGNAL( clicked(bool) ),                this, SIGNAL( changed() ) );
        connect( p_->ui.advCmdEdit,         SIGNAL( textChanged(const QString&)),   this, SIGNAL( changed() ) );

        connect( p_->ui.separate_x_check,   SIGNAL( stateChanged(int)),             this, SIGNAL( changed() ) );
    }
    else {
        disconnect( p_->ui.binary_edit,        SIGNAL( textChanged(const QString &) ), this, SIGNAL( changed() ) );

        disconnect( p_->ui.advCmdBox,          SIGNAL( clicked(bool) ),                this, SIGNAL( changed() ) );
        disconnect( p_->ui.advCmdEdit,         SIGNAL( textChanged(const QString&)),   this, SIGNAL( changed() ) );

        disconnect( p_->ui.separate_x_check,   SIGNAL( stateChanged(int)),             this, SIGNAL( changed() ) );
    }
}


void launch_settings_form::update_preferences()
{
    set_connections(false);

    app_settings as;
    p_->ui.binary_edit->setText( as.binary_path() );
    p_->ui.advCmdEdit->setText( as.adv_cmd_line() );
    p_->ui.advCmdBox->setChecked( as.use_adv_cmd_line() );
    p_->ui.separate_x_check->setChecked( as.separate_x() );

    set_connections(true);
}


void launch_settings_form::accept()
{
    app_settings as;
    as.set_binary_path( p_->ui.binary_edit->text() );
    as.set_use_adv_cmd_line( p_->ui.advCmdBox->isChecked() );
    as.set_adv_cmd_line( p_->ui.advCmdEdit->text() );
    as.set_separate_x( p_->ui.separate_x_check->isChecked() );
    
    update_preferences();
}


void launch_settings_form::reject()
{
    update_preferences();
}


void launch_settings_form::reset_defaults()
{
    p_->ui.binary_edit->clear();
    p_->ui.advCmdEdit->clear();

    accept();
}

void launch_settings_form::choose_binary()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Urban Terror executable"), "",
      tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fileName.isEmpty()) return;

    p_->ui.binary_edit->setText(fileName);
}

void launch_settings_form::insert_file_path()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Select file to insert"), "",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    p_->ui.advCmdEdit->insert(fileName);

    emit changed();
}

void launch_settings_form::adv_text_changed(const QString& text)
{
    launcher l;
    l.set_server_id(server_id("server:12345"));
    l.set_user_name("New_URT_Player");
    l.set_rcon("rcon_pAsSwOrD");
    l.set_password("pAsSwOrD");
    l.set_referee("referee_pAsSwOrD");

    p_->ui.adv_cmd_preview_edit->setText( l.launch_string() );
}

namespace {

void test_thread( dialog_syncer& syncer)
{
    boost::this_thread::sleep(boost::posix_time::seconds(2));
    if ( try_x_start() )
        syncer.accept();
    else
        syncer.reject();
}

}

void launch_settings_form::x_check()
{
    QProgressDialog progress( tr("This may take about 30 seconds"), QString(), 0, 0, this );
    dialog_syncer syncer;
    connect( &syncer, SIGNAL( accepted() ), &progress, SLOT( accept() ) );
    connect( &syncer, SIGNAL( rejected() ), &progress, SLOT( reject() ) );

    boost::thread t( boost::bind( test_thread, boost::ref(syncer) ) );

    p_->ui.separate_x_check->setChecked( progress.exec() );
    QMessageBox::information( this,
        tr( "X session autodetection" ),
        ( p_->ui.separate_x_check->isChecked() )
            ? tr( "Another X session started succesfully" )
            : tr( "Another X session failed" )
    );
}

