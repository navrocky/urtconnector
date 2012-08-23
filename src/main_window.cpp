#include <iostream>
#include <set>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#include <QAction>
#include <QTimer>
#include <QSettings>
#include <QMenu>
#include <QTreeView>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QMessageBox>
#include <QCloseEvent>
#include <QInputDialog>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QHeaderView>
#include <QProcess>
#include <QToolBar>
#include <QUuid>

#include <cl/syslog/manager.h>
#include <common/exception.h>
#include <common/qt_syslog.h>
#include <common/state_settings.h>
#include <common/server_list.h>
#include <common/qaccumulatingconnection.h>
#include <common/str_convert.h>
#include <common/main_tab_widget.h>
#include <common/item_view_dblclick_action_link.h>
#include <common/tools.h>
#include <settings/settings.h>
#include <launcher/launcher.h>
#include <launcher/tools.h>
#include <history/history.h>
#include <history/history_settings_form.h>
#include <jobs/job_monitor.h>
#include <rcon/rcon.h>
#include <friend_list/friend_prop_dialog.h>

#include <tracking/tasks_panel.h>
#include <tracking/db_saver.h>
#include <tracking/manager.h>
#include <tracking/context.h>
#include <tracking/pointers.h>
#include <tracking/task.h>
#include <tracking/conditions/reg_conditions.h>
#include <tracking/conditions/server_filter_condition.h>
#include <tracking/actions/reg_actions.h>
#include <tracking/actions/play_sound_action.h>
#include <tracking/actions/connect_action.h>
#include <tracking/actions/show_query_action.h>

#include <filters/filter_factory.h>
#include <filters/reg_filters.h>
#include <filters/filter_edit_widget.h>
#include <filters/filter_list.h>
#include <filters/composite_filter.h>
#include <filters/hide_full_filter.h>
#include <filters/hide_empty_filter.h>

#include "config.h"
#include "ui_main_window.h"
#include "server_options_dialog.h"
#include "push_button_action_link.h"
#include "about_dialog.h"
#include "app_options_saver.h"
#include "server_list_saver.h"
#include "server_info_html.h"
#include "job_update_selected.h"
#include "job_update_from_master.h"

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
#include <common/iconned_dock_style.h>
#endif

#include <anticheat/tools.h>
#include <anticheat/anticheat.h>
#include <anticheat/settings.h>
#include <anticheat/settings_widget.h>

//FIXME move this headers away
#include <preferences/src/preferences_dialog.h>

#include <rcon/rcon_settings_form.h>
#include <setting_forms/launch_settings_form.h>
#include <setting_forms/application_settings_form.h>
#include <setting_forms/update_settings_form.h>
#include <tabs/tab_context.h>
#include <tabs/bookmark_tab.h>
#include <tabs/server_list_tab.h>

#include "main_window.h"
#include "updater/update_task.h"

SYSLOG_MODULE(main_window)

using namespace std;
using boost::bind;

#if defined(Q_WS_X11)
/**
  Blur behind windows (on KDE4.5+)
  Uses a feature done for Plasma 4.5+ for hinting the window manager to draw
  blur behind the window.
*/
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <QX11Info>
static bool kde4EnableBlurBehindWindow(WId window, bool enable, const QRegion &region = QRegion())
{
    Display *dpy = QX11Info::display();
    Atom atom = XInternAtom(dpy, "_KDE_NET_WM_BLUR_BEHIND_REGION", False);

    if (enable) {
        QVector<QRect> rects = region.rects();
        QVector<quint32> data;
        for (int i = 0; i < rects.count(); i++) {
            const QRect r = rects[i];
            data << r.x() << r.y() << r.width() << r.height();
        }

        XChangeProperty(dpy, window, atom, XA_CARDINAL, 32, PropModeReplace,
                        reinterpret_cast<const unsigned char *>(data.constData()), data.size());
    } else {
        XDeleteProperty(dpy, window, atom);
    }
}
#endif


////////////////////////////////////////////////////////////////////////////////
// main_window

main_window::main_window(QWidget *parent)
: QMainWindow(parent)
, ui_(new Ui::MainWindowClass)
, all_sl_(new server_list)
, bookmarks_(new server_bookmark_list)
, history_sl_(new history)
, old_state_(0)
, clipper_( new clipper(this) )
, anticheat_(NULL)
, launcher_(new launcher(this))
{
//    setAttribute(Qt::WA_TranslucentBackground, true);
//    kde4EnableBlurBehindWindow(winId(), true);

    ui_->setupUi(this);

    ui_->server_info_browser->set_bookmarks( bookmarks_ );

    tab_toolbar_ = addToolBar(tr("Current tab toolbar"));
    tab_toolbar_->setObjectName("tab_toolbar");

    setWindowIcon( QIcon("images:logo.png") );

    tab_widget_ = new main_tab_widget(this);
    setCentralWidget(tab_widget_);

    server_info_updater_ = new QAccumulatingConnection(300, QAccumulatingConnection::Periodically, this);
    connect(server_info_updater_, SIGNAL(signal()), SLOT(update_server_info()));
    connect(all_sl_.get(), SIGNAL(changed()), server_info_updater_, SLOT(emitSignal()));
    connect(tab_widget_, SIGNAL(currentChanged(int)), server_info_updater_, SLOT(emitSignal()));

    anticheat_enabled_action_ = new QAction(QIcon("icons:anticheat.png"), tr("Enable anticheat"), this);
    anticheat_enabled_action_->setCheckable(true);

//    anticheat_open_action_ = new QAction(QIcon(":/images/icons/zoom.png"), tr("Enable anticheat"), this);
    anticheat_configure_action_ = new QAction(QIcon("icons:configure.png"), tr("Configure anticheat"), this);
    connect( anticheat_configure_action_, SIGNAL(triggered(bool)), SLOT(show_anticheat_options()) );

    copy_info_action_ = new QAction(QIcon("icons:edit-copy.png"),
                                    tr("Copy server info to clipboard"), this);
    copy_info_action_->setShortcut(QKeySequence::Copy);
    connect(copy_info_action_, SIGNAL(triggered()), SLOT(copy_info()));

    QMenu* m = new QMenu(this);
//    m->addAction(anticheat_open_action_);
    m->addAction(anticheat_configure_action_);
    anticheat_enabled_action_->setMenu(m);

    ui_->toolBar->addAction(anticheat_enabled_action_);
    ui_->toolBar->addAction(copy_info_action_);

    que_ = new job_queue(this);
    job_monitor* jm = new job_monitor(que_, this);
    ui_->status_bar->addPermanentWidget(jm);

    // initialize filter system
    filter_factory_ = filter_factory_p(new filter_factory);
    register_filters(filter_factory_);

    // Setting up system tray icon
    tray_menu_ = new QMenu(this);
    tray_menu_->addAction(ui_->actionShow);
    tray_menu_->addSeparator();
    tray_menu_->addAction(ui_->actionQuit);

    tray_ = new QSystemTrayIcon(this);
    tray_->setIcon(QIcon("images:logo.png"));
    tray_->show();
    tray_->setContextMenu(tray_menu_);
    tray_->setToolTip(tr("Click to show/hide UrTConnector or middle click to quick launch"));
    connect(tray_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(tray_activated(QSystemTrayIcon::ActivationReason)));
    connect(tray_, SIGNAL(messageClicked()), SLOT(raise_window()));

    ////////////////////////////////////////////////////////////////////////////
    // tracking subsystem
    using namespace tracking;
    update_dispatcher_ = new update_dispatcher(all_sl_, gi_, que_, this);

    track_ctx_.reset(new context_t(all_sl_, update_dispatcher_, tray_, this,
                                   boost::bind(&main_window::select_server, this, _1),
                                   &friends_));
    track_cond_factory_ = reg_conditions(track_ctx_);
    track_acts_factory_ = reg_actions(track_ctx_);

    track_man_ = new manager(this);
    track_db_saver_ = new db_saver(track_man_, track_cond_factory_,
                                   track_acts_factory_, this);

    tasks_panel* tp = new tasks_panel(track_man_, track_cond_factory_, track_acts_factory_, this);
    ui_->tasks_tracking_dock->setWidget(tp);
    connect(track_man_, SIGNAL(task_added(task_t*)), ui_->tasks_tracking_dock, SLOT(raise()));
    ////////////////////////////////////////////////////////////////////////////

    tab_context ctx(all_sl_, filter_factory_, bookmarks_, que_, &gi_,
                    ui_->actionConnect, track_man_, track_ctx_, &friends_);

    fav_list_ = new bookmark_tab("bookmarks_list", ctx, this);
    tab_widget_->add_widget(fav_list_);
    connect(fav_list_, SIGNAL(selection_changed()), SLOT(selection_changed()));

    all_list_ = new server_list_tab("full_list", ctx, this);
    tab_widget_->add_widget(all_list_);
    connect(all_list_, SIGNAL(selection_changed()), SLOT(selection_changed()));

    history_list_ = new history_widget(history_sl_, ctx, this);
    tab_widget_->add_widget(history_list_);
    connect(history_list_, SIGNAL(selection_changed()), SLOT(selection_changed()));

    friends_list_ = new friend_list_widget(&friends_, ctx, this);
    tab_widget_->add_widget(friends_list_);
    connect(friends_list_, SIGNAL(selection_changed()), SLOT(selection_changed()));

//    update_task* task = new update_task(this);
//    task->set_interval(20*1000);
//    update_dispatcher_->add_task(task);

    ////////////////////////////////////////////////////////////////////////////

    connect(tab_widget_, SIGNAL(currentChanged(int)), SLOT(current_tab_changed()));
    connect(ui_->actionOptions, SIGNAL(triggered()), SLOT(show_options()));
    connect(ui_->actionQuickConnect, SIGNAL(triggered()), SLOT(quick_connect()));
    connect(ui_->actionAbout, SIGNAL(triggered()), SLOT(show_about()));
    connect(ui_->actionConnect, SIGNAL(triggered()), SLOT(connect_selected()));
    connect(ui_->actionQuit, SIGNAL(triggered()), SLOT(quit_action()));
    connect(ui_->actionShow, SIGNAL(triggered()), SLOT(show_action()));
    connect(ui_->actionOpenRemoteConsole, SIGNAL(triggered()), SLOT(open_remote_console()));
    connect(clipper_, SIGNAL(info_obtained()), SLOT(clipboard_info_obtained()));
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), SLOT(commit_data_request(QSessionManager&)));
    connect(ui_->action_about_qt, SIGNAL(triggered()), SLOT(about_qt()));
    connect(ui_->quick_favorite_button, SIGNAL(clicked()), SLOT(quick_add_favorite()));
    connect(launcher_, SIGNAL(started()), SLOT(launcher_started()));
    connect(launcher_, SIGNAL(stopped()), SLOT(launcher_stopped()));

    new QAccumulatingConnection(bookmarks_.get(), SIGNAL(changed( const server_bookmark&, const server_bookmark& )), this, SLOT(save_bookmarks()), 10, QAccumulatingConnection::Finally, this);
    new push_button_action_link(this, ui_->quickConnectButton, ui_->actionQuickConnect);

    connect(ui_->server_info_browser, SIGNAL(add_to_friend(const player_info&)), SLOT(add_to_friend(const player_info&)));

    // loading all options
    load_all_at_start();
    load_geometry();

    update_actions();
    update_server_info();

    setVisible( !(app_settings().start_hidden()) );

    current_tab_changed();

    // launch tracking
    track_man_->start();
}

main_window::~main_window()
{
    delete track_db_saver_;
}

void main_window::clipboard_info_obtained()
{
    ui_->qlServerEdit->setText(clipper_->address());
    ui_->qlPasswordEdit->setText(clipper_->password());
    ui_->quick_connect_dock->raise();
    tray_->showMessage(tr("Server info catched"),
                       tr("Address: %1\nPassword: %2\n\n"
                       "Click this message to open UrTConnector.")
                       .arg(clipper_->address())
                       .arg(clipper_->password())
                       );
}

void main_window::raise_window()
{
    setVisible(true);
    setWindowState(windowState() & ~Qt::WindowMinimized | Qt::WindowActive);
    raise();
    activateWindow();
}

void main_window::show_options()
{
    app_settings as;

    preferences_dialog d( preferences_dialog::Auto, false, this );
    d.setWindowTitle(tr("Options"));
    d.setWindowIcon(QIcon("icons:configure.png"));
    d.add_item( new launch_settings_form() );
    d.add_item( new update_settings_form() );
    d.add_item( new application_settings_form() );
    d.add_item( new history_settings_form() );
    d.add_item( new rcon_settings_form() );
    d.add_item( new anticheat::settings_widget() );

    d.exec();

    update_geoip_database();
    history_list_->set_group_mode(as.history_grouping());
}

void main_window::update_geoip_database()
{
    try
    {
        gi_.set_database( app_settings().geoip_database() );
    }
    catch(const std::exception& e)
    {
        LOG_ERR << "GEOIP update error: " << e.what();
    }
}

void main_window::quick_connect()
{
    connect_to_server(server_id(ui_->qlServerEdit->text()),
                      ui_->qlPlayerEdit->text(),
                      ui_->qlPasswordEdit->text());
}

void main_window::quick_add_favorite()
{
    server_bookmark bm;
    bm.set_id(server_id(ui_->qlServerEdit->text()));
    bm.set_password(ui_->qlPasswordEdit->text());

    server_options_dialog d(this, bm);
    d.set_server_list(all_sl_);
    d.set_update_params( &gi_, que_ );

    d.update_name();
    if (d.exec() == QDialog::Rejected)
        return;
    bookmarks_->add(d.options());
}

void main_window::load_all_at_start()
{
    LOG_DEBUG << "Load state at start";
    load_server_bookmarks(get_app_options_settings("favorites"), bookmarks_.get());
    load_server_list("all_state", all_sl_);

    update_geoip_database();
    app_settings as;
    history_list_->set_group_mode(as.history_grouping());
}

void main_window::save_state_at_exit()
{
    LOG_DEBUG << "Save state at exit";

    save_geometry();
    save_server_list("all_state", all_sl_);
}

void main_window::save_bookmarks()
{
    save_server_bookmarks(get_app_options_settings("favorites"), bookmarks_.get());
}

void main_window::show_about()
{
    about_dialog d(this);
    d.exec();
}

void main_window::about_qt()
{
    QApplication::aboutQt();
}

server_id main_window::selected() const
{
    main_tab* tab = current_tab_widget();
    if (tab)
        return tab->selected_server();
    else
        return server_id();
}

server_info_p main_window::selected_info() const
{
    const server_id& id = selected();
    if (id.is_empty())
        return server_info_p();
    else
        return all_sl_->get(id);
}

void main_window::connect_to_server(const server_id& id,
                                    const QString& player_name,
                                    const QString& password)
{
    connect_to_server(id, player_name, password, app_settings().update_before_connect());
}

void main_window::connect_to_server(const server_id& id,
                                    const QString& player_name,
                                    const QString& password,
                                    bool check_before_connect)
{
    if (launcher_->is_started() || ac_proc_)
        throw qexception(tr("Game is launched already"));

    check_anticheat_prereq();
    const server_bookmark& bm = bookmarks_->get(id);

    app_settings as;
    // update server info
    if ( check_before_connect )
    {
        server_id_list idl;
        idl.push_back(id);
        job_p job(new job_update_selected(idl, all_sl_, gi_, tr("Update server info and launch a game")));
        que_->add_job(job);
        job->wait_for_finish();
        if (job->is_canceled())
            return;
    }

    server_info_p info = all_sl_->get(id);

    // show warnings about connect
    // TODO also need to check info age by timestamp later
    if ( info && info->status == server_info::s_up )
    {
        QString msg;

        int max_slots = info->max_player_count;

        // with a rcon and referee you can connect to a private slots
        if (bm.rcon_password().isEmpty() && bm.ref_password().isEmpty())
            max_slots = info->public_slots();

        if (info->players.size() >= max_slots)
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Question);
            msg.setWindowTitle(tr("Connecting to the server"));
            msg.setWindowIcon(QIcon("icons:urtconnector.png"));
            msg.setText(tr("Server is full."));
            msg.setInformativeText(tr("Would you like to wait until there will "
                                      "be empty slots or to connect right now?"));
            QPushButton* wait_btn = msg.addButton(tr("Wait for empty slots"), QMessageBox::AcceptRole);
            wait_btn->setIcon(QIcon("icons:chronometer.png"));
            QPushButton* connect_btn = msg.addButton(tr("Connect right now"), QMessageBox::AcceptRole);
            connect_btn->setIcon(QIcon("icons:launch.png"));
            QPushButton* cancel_btn = msg.addButton(QMessageBox::Cancel);
            msg.setDefaultButton(wait_btn);
            msg.exec();
            if (msg.clickedButton() == wait_btn)
            {
                create_waiting_task(id);
                return;
            }
            if (msg.clickedButton() == cancel_btn || msg.clickedButton() == 0)
                return;
        }

        if (info->players.size() == 0)
        {
            QMessageBox msg;
            msg.setIcon(QMessageBox::Question);
            msg.setWindowTitle(tr("Connecting to the server"));
            msg.setWindowIcon(QIcon("icons:urtconnector.png"));
            msg.setText(tr("Server is empty."));
            msg.setInformativeText(tr("Do you want to wait when someone appear on this server or to connect right now?"));
            QPushButton* wait_btn = msg.addButton(tr("Wait for someone"), QMessageBox::AcceptRole);
            wait_btn->setIcon(QIcon("icons:chronometer.png"));
            QPushButton* connect_btn = msg.addButton(tr("Connect right now"), QMessageBox::AcceptRole);
            connect_btn->setIcon(QIcon("icons:launch.png"));
            QPushButton* cancel_btn = msg.addButton(QMessageBox::Cancel);
            msg.setDefaultButton(wait_btn);
            msg.exec();
            if (msg.clickedButton() == wait_btn)
            {
                create_waiting_someone_task(id);
                return;
            }
            if (msg.clickedButton() == cancel_btn || msg.clickedButton() == 0)
                return;
        }
    }

    // take the password
    QString pass = password;
    if (pass.isEmpty() && !bm.password().isEmpty())
        pass = bm.password();

    if ( pass.isEmpty() && info && info->is_password_needed() )
    {
        bool ok;
        pass = QInputDialog::getText(this, tr("This server require the password"),
            tr("Enter the password"), QLineEdit::Normal, "", &ok );
        if (!ok)
            return;
    }

    // add to the history
    QString server_name = info ? info->name : QString();
    history_sl_->add(id, server_name, player_name, pass);

#if defined(Q_OS_UNIX)
    if ( anticheat_enabled_action_->isChecked() && as.separate_xsession() )
    {
        QStringList args;
        args << qApp->applicationFilePath();

        // debug logging enabled?
        if (cl::syslog::logman().level_check(cl::syslog::debug))
            args << "--debug";

        // common options
        args << "--anticheat" << "--launch" << "--pipe-log"
                << "--player" << player_name
                << "--addr"<< id.address();

        if (!pass.isEmpty())
            args << "--pass" << pass;
        if (!bm.rcon_password().isEmpty())
            args << "--rcon" << bm.rcon_password();
        if (!bm.ref_password().isEmpty())
            args << "--referee" << bm.ref_password();

        args << "--" << QString(":%1").arg(find_free_display());

        // TODO log redirection of subprocess

        ac_proc_ = new QProcess(this);

        // self destruct on finish
        connect(ac_proc_, SIGNAL(error(QProcess::ProcessError)), ac_proc_, SLOT(deleteLater()));
        connect(ac_proc_, SIGNAL(finished(int,QProcess::ExitStatus)), ac_proc_, SLOT(deleteLater()));

        ac_proc_->start("xinit", args);

        return;
    }
#endif

    bool is_urt42 = info->game_type == "q3urt42";

    QString binary_path = is_urt42 ? as.binary_path_42() : as.binary_path();
    QString adv_cmd_line = is_urt42 ? (as.use_adv_cmd_line_42() ? as.adv_cmd_line_42() : QString())
                                    : (as.use_adv_cmd_line() ? as.adv_cmd_line() : QString()) ;

    QString version = is_urt42 ? "4.2" : "4.1";

    if (binary_path.isEmpty() || !QFileInfo(binary_path).exists() || !QFileInfo(binary_path).isExecutable())
    {
        QMessageBox::information(this, tr("UrbanTerror %1 executable missing").arg(version),
                                 tr("No UrbanTerror %1 executable found, please select executable manually").arg(version));
        forever
        {
            binary_path = QFileDialog::getOpenFileName(this, tr("Please select UrbanTerror %1 executable").arg(version));
            if (binary_path.isNull())
                return;

            if (!QFileInfo(binary_path).exists())
            {
                QMessageBox::critical(this, tr("Error"), tr("File %1 doesn't exists").arg(binary_path));
                continue;
            }
            if (!QFileInfo(binary_path).isExecutable())
            {
                QMessageBox::critical(this, tr("Error"), tr("File %1 doesn't executable").arg(binary_path));
                continue;
            }

            if (is_urt42)
                as.binary_path_42_set(binary_path);
            else
                as.binary_path_set(binary_path);
            break;
        }
    }

    launcher* l = launcher_;
    l->set_server_id(id);
    l->set_user_name(player_name);
    l->set_password(pass);
    l->set_referee(bm.ref_password());
    l->set_rcon(bm.rcon_password());
    l->set_binary_path(binary_path);
    l->set_advanced_cmd_line(adv_cmd_line);
    l->set_separate_x(as.separate_xsession());
    l->set_mumble_overlay(as.use_mumble_overlay());
    l->set_mumble_overlay_bin(as.mumble_overlay_bin());
    l->set_detach(as.multiple_launch() || as.after_launch_action() == al_quit);
    l->launch();

    switch (as.after_launch_action())
    {
        case al_none: break;
        case al_hide: hide(); break;
        case al_quit: quit_action(); break;
    }
}

void main_window::connect_selected()
{
    server_id id = selected();
    if (id.is_empty())
        return;
    const QString& player_name = ui_->qlPlayerEdit->text();
    connect_to_server(id, player_name, QString());
}

main_tab* main_window::current_tab_widget() const
{
    QWidget* curw = tab_widget_->currentWidget();
    main_tab* res = qobject_cast<main_tab*>(curw);
    return res;
}

void main_window::update_actions()
{
//    main_tab* cw = current_tab_widget();
    bool sel = !(selected().is_empty());

    ui_->actionConnect->setEnabled(sel);
    ui_->actionOpenRemoteConsole->setEnabled(sel);
    copy_info_action_->setEnabled(sel);
}

void main_window::current_tab_changed()
{
    update_actions();

    // update tab toolbar actions
    tab_toolbar_->clear();
    if (main_tab* tab = current_tab_widget()){
        tab_toolbar_->addActions(tab->actions());    
    }
}

void main_window::save_geometry()
{
    state_settings state_s;
    state_s.geometry_set( saveGeometry() );

    qsettings_p s = base_settings::get_settings( state_settings::uid() );
    s->setValue("window_state", saveState());
    fav_list_->save_state();
    all_list_->save_state();
    history_list_->save_state();
}

void main_window::load_geometry()
{
    qsettings_p s = base_settings::get_settings( state_settings::uid() );

    state_settings state_s;
    restoreGeometry( state_s.geometry() );

    qApp->processEvents();
    restoreState(s->value("window_state").toByteArray());

    fav_list_->load_state();
    all_list_->load_state();
    history_list_->load_state();
}

void main_window::update_server_info()
{
    if (!ui_->server_info_dock->isVisible())
        return;

    server_info_p si = selected_info();
    if (si)
    {
        if (old_id_ == si->id && old_state_ == si->update_stamp())
            return;

        old_state_ = si->update_stamp();
        old_id_ = si->id;

        ui_->server_info_browser->set_server_info(si);
    }
    else
    {
        old_id_ = server_id();
        old_state_ = 0;
        ui_->server_info_browser->set_server_info( server_info_p() );
    }
}

void main_window::selection_changed()
{
    update_actions();
    server_info_updater_->emitSignal();
}

void main_window::add_selected_to_fav()
{
    server_info_p si = selected_info();
    if (!si) return;

    server_bookmark bm;
    bm.set_id(si->id);
    bm.set_name(si->name);

    server_options_dialog d(this, bm);
    d.set_server_list(all_sl_);
    d.set_update_params(&gi_, que_);
    if (d.exec() == QDialog::Rejected) return;

    bookmarks_->add(d.options());
}

void main_window::show_action()
{
    setVisible(!isVisible());
}

void main_window::commit_data_request(QSessionManager&)
{
    LOG_DEBUG << "Commit data request";
    save_state_at_exit();
}

void main_window::quit_action()
{
    if (launcher_->is_started())
    {
        if (QMessageBox::question(this, tr("Quit"), tr("The game is started at this moment.\n\nKill the game and quit now?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
            return;
    }

    LOG_DEBUG << "Quit action";
    launcher_->stop();
    hide();
    tray_->hide();
    save_state_at_exit();
    qApp->quit();

}

void main_window::tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        ui_->actionShow->trigger();
    }
    if (reason == QSystemTrayIcon::MiddleClick)
    {
        ui_->actionQuickConnect->trigger();
    }
}

void main_window::clear_all()
{
    LOG_HARD << "Deleting all entries in server list";
    all_sl_->remove_all();
}

void main_window::clear_selected()
{
    LOG_HARD << "Deleting selected entries";
    all_sl_->remove_selected(all_list_->selection());
}

void main_window::open_remote_console()
{
    const server_id& id = selected();
    if (id.is_empty())
        return;

    // TODO query password from user if it not stored in bookmarks

    QDockWidget* dw = new QDockWidget( tr("RCon : %1").arg(id.address()), this );
    dw->setAttribute( Qt::WA_DeleteOnClose  );
    dw->setWidget( new rcon(this, id, bookmarks_->get(id)) );

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    dw->setStyle( new iconned_dock_style( QIcon("icons:utilities-terminal.png"), dw->style() ) );
#endif
    addDockWidget(Qt::BottomDockWidgetArea,  dw );
}

void main_window::launcher_started()
{
    LOG_DEBUG << "Game started";
    update_dispatcher_->set_game_started(true);
    if (!anticheat_enabled_action_->isChecked())
        return;

    QString player_name = ui_->qlPlayerEdit->text();

    anticheat_ = anticheat::create_anticheat(player_name, this);
    anticheat_->start();
}

void main_window::launcher_stopped()
{
    LOG_DEBUG << "Game stopped";
    update_dispatcher_->set_game_started(false);
    delete anticheat_;
}

void main_window::check_anticheat_prereq() const
{
    if (!anticheat_enabled_action_->isChecked())
        return;
    QString player_name = ui_->qlPlayerEdit->text();
    if (player_name.isEmpty())
        throw qexception(tr("The player's name must be defined in the quick launch window for the anti-cheat!"));
}

void main_window::show_anticheat_options()
{
    preferences_dialog d( preferences_dialog::Auto, false, this );
    d.setWindowTitle(tr("Anticheat options"));
    d.setWindowIcon(QIcon("icons:anticheat.png"));
    d.add_item( new anticheat::settings_widget(&d) );
    d.exec();
}

void main_window::add_to_friend( const player_info& player )
{
    friend_prop_dialog d(this);
    friend_record fr;
    fr.set_nick_name(player.nick_name());
    d.set_rec(fr);
    if (d.exec() != QDialog::Accepted)
        return;
    friends_.add(fr);
}

void main_window::copy_info()
{
    const server_id& id = selected();
    if (id.is_empty())
        return;

    QApplication::clipboard()->setText(
        QString("/connect %1; password %2")
        .arg(id.address())
        .arg(bookmarks_->get(id).password())
    );
}


void main_window::select_server(const server_id& id)
{
    tab_widget_->setCurrentIndex(1);
    all_list_->set_selected_server(id);
}

void main_window::create_waiting_task(const server_id& id)
{
    using namespace tracking;

    server_info_p si = all_sl_->get(id);

    // creating task
    task_t* task = new task_t(this);
    task->set_caption(tr("Connect to %1").arg(si ? si->name : id.address()));
    task->set_operation_mode(task_t::om_destroy_after_trigger);
    QUuid uid = QUuid::createUuid();
    task->set_id(uid.toString());

    // assign filter condition to the task
    condition_class_p cc(new server_filter_condition_class(track_ctx_));
    condition_p cond = cc->create();
    server_filter_condition* sfc = dynamic_cast<server_filter_condition*>(cond.get());
    task->set_condition(cond);
    sfc->set_interval(5000);
    sfc->set_servers(id.address());
    sfc->set_use_auto_update(true);

    // add free slots filter
    composite_filter* cf = dynamic_cast<composite_filter*>(sfc->filters()->root_filter().get());

    filter_class_p fc(new hide_full_filter_class);
    filter_p flt = fc->create_filter();
    flt->set_name(QUuid::createUuid().toString());
    cf->add_filter(flt);

    // add play sound action
    action_class_p ac(new play_sound_action_class(track_ctx_));
    action_p a = ac->create();
    play_sound_action* psa = static_cast<play_sound_action*>(a.get());
    psa->set_sound_file(app_settings().notification_sound());
    task->add_action(a);

    // add query action
    ac.reset(new show_query_action_class(track_ctx_));
    a = ac->create();
    show_query_action* qa = dynamic_cast<show_query_action*>(a.get());
    qa->set_title(tr("Connect"));
    qa->set_message(tr("On the server <b>%server</b> appeared empty slots.<br><br>Do you wish to connect?"));
    task->add_action(a);

    // add connect action
    ac.reset(new connect_action_class(track_ctx_));
    a = ac->create();
    task->add_action(a);

    track_man_->add_task(task);
    task->condition()->start();
    
}

void main_window::create_waiting_someone_task(const server_id& id)
{
    using namespace tracking;

    server_info_p si = all_sl_->get(id);

    // creating task
    task_t* task = new task_t(this);
    task->set_caption(tr("Connect to %1 if someone appeared").arg(si ? si->name : id.address()));
    task->set_operation_mode(task_t::om_destroy_after_trigger);
    QUuid uid = QUuid::createUuid();
    task->set_id(uid.toString());

    // assign filter condition to the task
    condition_class_p cc(new server_filter_condition_class(track_ctx_));
    condition_p cond = cc->create();
    server_filter_condition* sfc = dynamic_cast<server_filter_condition*>(cond.get());
    task->set_condition(cond);
    sfc->set_interval(5000);
    sfc->set_servers(id.address());
    sfc->set_use_auto_update(true);

    // add someone appeared filter
    composite_filter* cf = dynamic_cast<composite_filter*>(sfc->filters()->root_filter().get());

    filter_class_p fc(new hide_empty_filter_class);
    filter_p flt = fc->create_filter();
    flt->set_name(QUuid::createUuid().toString());
    cf->add_filter(flt);

    // add play sound action
    action_class_p ac(new play_sound_action_class(track_ctx_));
    action_p a = ac->create();
    play_sound_action* psa = static_cast<play_sound_action*>(a.get());
    psa->set_sound_file(app_settings().notification_sound());
    task->add_action(a);

    // add query action
    ac.reset(new show_query_action_class(track_ctx_));
    a = ac->create();
    show_query_action* qa = dynamic_cast<show_query_action*>(a.get());
    qa->set_title(tr("Connect"));
    qa->set_message(tr("On the server <b>%server</b> someone appeared.<br><br>Do you wish to connect him?"));
    task->add_action(a);

    // add connect action
    ac.reset(new connect_action_class(track_ctx_));
    a = ac->create();
    task->add_action(a);

    track_man_->add_task(task);
    task->condition()->start();
}
