#include "launch_settings_form.h"

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include <QFileDialog>
#include <QProgressDialog>
#include <QMessageBox>

#include "app_options.h"
#include <launcher/launcher.h>
#include <launcher/tools.h>
#include <common/scoped_tools.h>

#include "launch_opts_panel.h"
#include "ui_launch_settings_form.h"


struct launch_settings_form::Pimpl{
    Ui_launch_settings_form ui;
};

launch_settings_form::launch_settings_form(QWidget* parent, Qt::WindowFlags f)
: preferences_widget(parent, tr("Launch"))
, p_( new Pimpl )
, lock_change_(false)
{
    p_->ui.setupUi(this);

    set_icon( QIcon("images:logo.png") );
    set_header( tr("UrbanTerror launch settings") );

    connect(p_->ui.urt41_panel, SIGNAL(changed()), SIGNAL(changed()));
    connect(p_->ui.urt42_panel, SIGNAL(changed()), SIGNAL(changed()));

    connect( p_->ui.x_check_button, SIGNAL(clicked()), this, SLOT( x_check() ));
    connect( p_->ui.select_mumble_bin_button, SIGNAL(clicked()), this, SLOT( choose_mumble_bin() ));

    connect( p_->ui.separate_x_check, SIGNAL(stateChanged(int)), this, SLOT( int_changed() ) );
    connect( p_->ui.update_server_check, SIGNAL(stateChanged(int)), this, SLOT( int_changed() ) );
    connect( p_->ui.multiple_launch_check, SIGNAL(stateChanged(int)), this, SLOT( int_changed() ) );
    connect( p_->ui.after_launch_combo, SIGNAL(currentIndexChanged(int)), this, SLOT( int_changed() ) );
    connect( p_->ui.mumble_overlay_group, SIGNAL(toggled(bool)), this, SLOT( int_changed() ) );
    connect( p_->ui.mumble_bin_edit, SIGNAL(textChanged(QString)), this, SLOT( int_changed() ) );

#ifndef Q_OS_UNIX
    p_->ui.separate_x_check->setVisible(false);
    p_->ui.x_check_button->setVisible(false);
    p_->ui.mumble_overlay_group->setVisible(false);
#endif
}

void launch_settings_form::int_changed()
{
    launch_opts_panel* p41 = p_->ui.urt41_panel;
    launch_opts_panel* p42 = p_->ui.urt42_panel;
    p41->set_separate_xsession(p_->ui.separate_x_check->isChecked());
    p41->set_use_mumble_overlay(p_->ui.mumble_overlay_group->isChecked());
    p41->set_mumble_overlay_bin(p_->ui.mumble_bin_edit->text());
    p42->set_separate_xsession(p_->ui.separate_x_check->isChecked());
    p42->set_use_mumble_overlay(p_->ui.mumble_overlay_group->isChecked());
    p42->set_mumble_overlay_bin(p_->ui.mumble_bin_edit->text());
    if (!lock_change_)
        emit changed();
}

void launch_settings_form::update_preferences()
{
    scoped_value_change<bool> s(lock_change_, true, false);

    app_settings as;

    launch_opts_panel* p41 = p_->ui.urt41_panel;
    launch_opts_panel* p42 = p_->ui.urt42_panel;

    p41->set_binary_path(as.binary_path());
    p42->set_binary_path(as.binary_path_42());
    p41->set_adv_cmd_line( as.adv_cmd_line() );
    p42->set_adv_cmd_line( as.adv_cmd_line_42() );
    p41->set_use_adv_cmd_line( as.use_adv_cmd_line() );
    p42->set_use_adv_cmd_line( as.use_adv_cmd_line_42() );
    p_->ui.update_server_check->setChecked( as.update_before_connect() );
    p_->ui.multiple_launch_check->setChecked( as.multiple_launch() );
    p_->ui.after_launch_combo->setCurrentIndex( as.after_launch_action() );
#if defined(Q_OS_UNIX)
    p_->ui.separate_x_check->setChecked( as.separate_xsession() );
    p41->set_separate_xsession( as.separate_xsession() );
    p42->set_separate_xsession( as.separate_xsession() );

    p_->ui.mumble_overlay_group->setChecked( as.use_mumble_overlay() );
    p41->set_use_mumble_overlay( as.use_mumble_overlay() );
    p42->set_use_mumble_overlay( as.use_mumble_overlay() );

    p_->ui.mumble_bin_edit->setText( as.mumble_overlay_bin() );
    p41->set_mumble_overlay_bin( as.mumble_overlay_bin() );
    p42->set_mumble_overlay_bin( as.mumble_overlay_bin() );
#endif
}

void launch_settings_form::accept()
{
    app_settings as;

    launch_opts_panel* p41 = p_->ui.urt41_panel;
    launch_opts_panel* p42 = p_->ui.urt42_panel;

    as.binary_path_set( p41->binary_path() );
    as.binary_path_42_set( p42->binary_path() );
    as.use_adv_cmd_line_set( p41->use_adv_cmd_line() );
    as.use_adv_cmd_line_42_set( p42->use_adv_cmd_line() );
    as.adv_cmd_line_set( p41->adv_cmd_line() );
    as.adv_cmd_line_42_set( p42->adv_cmd_line() );
    as.update_before_connect_set(p_->ui.update_server_check->isChecked());
    as.multiple_launch_set(p_->ui.multiple_launch_check->isChecked());
    as.after_launch_action_set(p_->ui.after_launch_combo->currentIndex());
#if defined(Q_OS_UNIX)
    as.separate_xsession_set( p_->ui.separate_x_check->isChecked() );
    as.use_mumble_overlay_set(p_->ui.mumble_overlay_group->isChecked());
    as.mumble_overlay_bin_set( p_->ui.mumble_bin_edit->text() );
#endif
    update_preferences();
}


void launch_settings_form::reject()
{
    update_preferences();
}

void launch_settings_form::reset_defaults()
{
    app_settings as;
    as.binary_path_reset();
    as.binary_path_42_reset();
    as.use_adv_cmd_line_reset();
    as.use_adv_cmd_line_42_reset();
    as.adv_cmd_line_reset();
    as.adv_cmd_line_42_reset();
    as.update_before_connect_reset();
    as.multiple_launch_reset();
    as.after_launch_action_reset();
#if defined(Q_OS_UNIX)
    as.separate_xsession_reset();
    as.use_mumble_overlay_reset();
    as.mumble_overlay_bin_reset();
#endif
    update_preferences();
}

void launch_settings_form::choose_mumble_bin()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("mumble-overlay binary"), "/usr/bin",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    p_->ui.mumble_bin_edit->setText(fileName);
}

namespace
{

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
            : tr( "Another X session failed.\n\nHint: check that the suid bit is enabled on your Xorg binary." )
    );
}

