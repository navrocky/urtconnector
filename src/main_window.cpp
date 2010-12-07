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
#include <QHeaderView>

#include <common/exception.h>
#include <common/qt_syslog.h>
#include <common/state_settings.h>
#include <common/server_list.h>
#include <common/qaccumulatingconnection.h>
#include <settings/settings.h>
#include <anticheat/tools.h>
#include <launcher/launcher.h>

#include "config.h"
#include "ui_main_window.h"
#include "options_dialog.h"
#include "server_options_dialog.h"
#include "push_button_action_link.h"
#include "about_dialog.h"
#include "app_options_saver.h"
#include "server_list_saver.h"
#include "server_info_html.h"
#include "item_view_dblclick_action_link.h"
#include "str_convert.h"
#include "tools.h"
#include <history/history.h>

#include <jobs/job_monitor.h>
#include "job_update_selected.h"
#include "job_update_from_master.h"

#include <rcon/rcon.h>

#include <filters/filter_factory.h>
#include <filters/reg_filters.h>
#include <filters/filter_edit_widget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
#include <common/iconned_dock_style.h>
#endif

#include <anticheat/anticheat.h>
#include <anticheat/settings.h>

#include "main_window.h"

SYSLOG_MODULE(main_window)

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main_window

main_window::main_window(QWidget *parent)
: QMainWindow(parent)
, ui_(new Ui::MainWindowClass)
, opts_(new app_options())
, all_sl_(new server_list)
, bookmarks_(new server_bookmark_list(this))
, history_sl_(new history(opts_))
, old_state_(0)
, clipper_( new clipper(this, opts_) )
, anticheat_(NULL)
, launcher_(new launcher(opts_, this))
{
//    setAttribute(Qt::WA_TranslucentBackground, true);
    ui_->setupUi(this);

    anticheat_enabled_action_ = new QAction(QIcon(":/icons/icons/anticheat.png"), tr("Enable anticheat"), this);
    anticheat_enabled_action_->setCheckable(true);

//    anticheat_open_action_ = new QAction(QIcon(":/images/icons/zoom.png"), tr("Enable anticheat"), this);
    anticheat_configure_action_ = new QAction(QIcon(":/icons/icons/configure.png"), tr("Configure anticheat"), this);
    QMenu* m = new QMenu(this);
//    m->addAction(anticheat_open_action_);
    m->addAction(anticheat_configure_action_);
    anticheat_enabled_action_->setMenu(m);

    ui_->toolBar->addAction(anticheat_enabled_action_);

    que_ = new job_queue(this);
    job_monitor* jm = new job_monitor(que_, this);
    ui_->status_bar->addPermanentWidget(jm);

    // initialize filter system
    filter_factory_ = filter_factory_p(new filter_factory);
    register_filters(filter_factory_);

#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
    ui_->tabWidget->setDocumentMode(true);
#endif

    // Setting up system tray icon
    tray_menu_ = new QMenu(this);
    tray_menu_->addAction(ui_->actionShow);
    tray_menu_->addSeparator();
    tray_menu_->addAction(ui_->actionQuit);
    
    tray_ = new QSystemTrayIcon(this);
    tray_->setIcon(QIcon(":/images/icons/logo.png"));
    tray_->show();
    tray_->setContextMenu(tray_menu_);
    tray_->setToolTip(tr("Click to show/hide UrTConnector or middle click to quick launch"));
    connect(tray_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(tray_activated(QSystemTrayIcon::ActivationReason)));
    connect(tray_, SIGNAL(messageClicked()), SLOT(raise_window()));        

    serv_info_update_timer_ = new QTimer(this);
    serv_info_update_timer_->setInterval(1000);
    connect(serv_info_update_timer_, SIGNAL(timeout()), SLOT(update_server_info()));
    serv_info_update_timer_->start();

    all_list_ = new server_list_widget(opts_, filter_factory_, ui_->tabAll);
    all_list_->setObjectName("all_list");
    QBoxLayout* l = dynamic_cast<QBoxLayout*>(ui_->tabAll->layout());
    l->insertWidget(0, all_list_);
    connect(all_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    fav_list_ = new server_list_widget(opts_, filter_factory_, ui_->tabFav);
    fav_list_->setObjectName("fav_list");
    dynamic_cast<QBoxLayout*>(ui_->tabFav->layout())->insertWidget(0, fav_list_);
    connect(fav_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(current_tab_changed(int)));
    connect(ui_->actionOptions, SIGNAL(triggered()), SLOT(show_options()));
    connect(ui_->actionQuickConnect, SIGNAL(triggered()), SLOT(quick_connect()));
    connect(ui_->actionFavAdd, SIGNAL(triggered()), SLOT(fav_add()));
    connect(ui_->actionFavEdit, SIGNAL(triggered()), SLOT(fav_edit()));
    connect(ui_->actionFavDelete, SIGNAL(triggered()), SLOT(fav_delete()));
    
    connect(ui_->actionRefreshSelected, SIGNAL(triggered()), SLOT(refresh_selected()));
    connect(ui_->actionRefreshAll, SIGNAL(triggered()), SLOT(refresh_all_bookmarks()));
    connect(ui_->actionRefreshMaster, SIGNAL(triggered()), SLOT(refresh_master()));
    
    connect(ui_->actionAbout, SIGNAL(triggered()), SLOT(show_about()));
    connect(ui_->actionConnect, SIGNAL(triggered()), SLOT(connect_selected()));
    connect(ui_->actionAddToFav, SIGNAL(triggered()), SLOT(add_selected_to_fav()));
    connect(ui_->actionQuit, SIGNAL(triggered()), SLOT(quit_action()));
    connect(ui_->actionShow, SIGNAL(triggered()), SLOT(show_action()));
    connect(ui_->actionClearAll, SIGNAL(triggered()), SLOT(clear_all()));
    connect(ui_->actionClearSelected, SIGNAL(triggered()), SLOT(clear_selected()));
    
    connect(ui_->actionOpenRemoteConsole, SIGNAL(triggered()), SLOT(open_remote_console()));

    connect(clipper_, SIGNAL(info_obtained()), SLOT(clipboard_info_obtained()));
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), SLOT(commit_data_request(QSessionManager&)));
    connect(ui_->action_about_qt, SIGNAL(triggered()), SLOT(about_qt()));
    connect(ui_->quick_favorite_button, SIGNAL(clicked()), SLOT(quick_add_favorite()));
    connect(launcher_, SIGNAL(started()), SLOT(launcher_started()));
    connect(launcher_, SIGNAL(stopped()), SLOT(launcher_stopped()));

    new QAccumulatingConnection(bookmarks_, SIGNAL(changed()), this, SLOT(save_bookmarks()), 10, QAccumulatingConnection::Finally, this);
    new push_button_action_link(this, ui_->quickConnectButton, ui_->actionQuickConnect);

    all_list_->set_server_list(all_sl_);
    all_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    all_list_->tree()->addAction(ui_->actionConnect);
    add_separator_action(all_list_->tree());
    all_list_->tree()->addAction(ui_->actionAddToFav);
    add_separator_action(all_list_->tree());
    all_list_->tree()->addAction(ui_->actionRefreshSelected);
    all_list_->tree()->addAction(ui_->actionRefreshMaster);
    
    new item_view_dblclick_action_link(this, all_list_->tree(), ui_->actionConnect);

    fav_list_->set_server_list(all_sl_);
    fav_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    fav_list_->tree()->addAction(ui_->actionConnect);
    add_separator_action(fav_list_->tree());
    fav_list_->tree()->addAction(ui_->actionFavAdd);
    fav_list_->tree()->addAction(ui_->actionFavEdit);
    fav_list_->tree()->addAction(ui_->actionFavDelete);
    add_separator_action(fav_list_->tree());
    fav_list_->tree()->addAction(ui_->actionRefreshSelected);
    fav_list_->tree()->addAction(ui_->actionRefreshAll);

    new item_view_dblclick_action_link(this, fav_list_->tree(), ui_->actionConnect);

    fav_list_->set_bookmarks(bookmarks_);
    //all_list_->set_favs(&(opts_->servers));
    // history_list_->set_favs(&(opts_->servers));

    // loading all options
    load_all_at_start();
    load_geometry();

    // history
    history_sl_->change_max();
    load_history_tab();

    update_actions();

//    sync_fav_list();
    update_server_info();
    setVisible(!(opts_->start_hidden));
    current_tab_changed( ui_->tabWidget->currentIndex() );

    all_list_->force_update();
    fav_list_->force_update();
    update_tabs();
}

main_window::~main_window()
{
}

void main_window::clipboard_info_obtained()
{
    ui_->qlServerEdit->setText(clipper_->address());
    ui_->qlPasswordEdit->setText(clipper_->password());
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
    bool wasHistoryEnabled = opts_->keep_history;
    unsigned int oldNumberInHistory = opts_->number_in_history;

    options_dialog d;
    d.set_opts(opts_);
    if (d.exec() == QDialog::Rejected) return;
    save_options();

    if (wasHistoryEnabled != opts_->keep_history)
    {
        load_history_tab();
    }
    if (oldNumberInHistory != opts_->number_in_history)
    {
        history_sl_->change_max();
        if (opts_->keep_history)
        {
            history_list_->update_history();
        }
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
    server_bookmark opts;
    opts.id = server_id(ui_->qlServerEdit->text());
    opts.password = ui_->qlPasswordEdit->text();

    server_options_dialog d(this, opts);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    d.update_name();
    if (d.exec() == QDialog::Rejected)
        return;
    bookmarks_->add(d.options());
}

void main_window::fav_add()
{
    server_options_dialog d(this);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    if (d.exec() == QDialog::Rejected)
        return;
    bookmarks_->add(d.options());
}

void main_window::fav_edit()
{
    server_id id = selected();
    if (id.is_empty())
        return;
    const server_bookmark& bm = bookmarks_->get(id);
    server_options_dialog d(this, bm);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    if (d.exec() == QDialog::Rejected)
        return;
    bookmarks_->change(id, d.options());
}

void main_window::fav_delete()
{
    if (fav_list_->selection().size() == 0)
        return;
    if (QMessageBox::question(this, tr("Delete a favorite"), tr("Delete selected favorites?"),
                              QMessageBox::Yes | QMessageBox::No, QMessageBox::No) != QMessageBox::Yes)
        return;

    foreach (const server_id& id, fav_list_->selection())
    {
        bookmarks_->remove(id);
    }
}

void main_window::save_options()
{
    save_app_options(get_app_options_settings("options"), opts_);
}

void main_window::load_all_at_start()
{
    struct local
    {
        static void load_list(server_list_p list, const QString& name)
        {
            load_server_list(get_app_options_settings(name), name, list);
        }
    };
    
#if defined(Q_OS_UNIX)
    QString default_qstat = "/usr/bin/qstat";
#elif defined(Q_OS_WIN)
    QString default_qstat = "qstat.exe";
#elif defined(Q_OS_MAC)
    QString default_qstat = "/usr/bin/qstat";
#endif
    QString default_database = QString(URT_DATADIR) + "GeoIP.dat";

    opts_->qstat_opts.master_server = "master.urbanterror.net";
    opts_->qstat_opts.qstat_path = default_qstat;
    opts_->geoip_database = default_database;
    
    opts_->looking_for_clip = true;
    opts_->lfc_regexp = "(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})"
        "(:(\\d{1,5}))?(.+pass[^\\s]*\\s+([^\\s]+))?";
    opts_->lfc_host = 1;
    opts_->lfc_port = 3;
    opts_->lfc_password = 5;

    load_app_options(get_app_options_settings("options"), opts_);
    load_server_bookmarks(get_app_options_settings("favorites"), bookmarks_);

    local::load_list(all_sl_, "all_state");
//    local::load_list(fav_sl_, "favs_state");
    
    all_list_->load_options();
    fav_list_->load_options();
}

void main_window::save_state_at_exit()
{
    struct local
    {
        static void save_list(server_list_p list, const QString& name)
        {
            qsettings_p s = get_app_options_settings(name);
            QFile::remove(s->fileName());
            save_server_list(s, name, list);
        }
    };

    LOG_DEBUG << "Save state at exit";

    save_geometry();
    local::save_list(all_sl_, "all_state");
//    local::save_list(fav_sl_, "favs_state");

    all_list_->save_options();
    fav_list_->save_options();
}

void main_window::save_bookmarks()
{
    save_server_bookmarks(get_app_options_settings("favorites"), bookmarks_);
}

void main_window::refresh_servers(server_list_widget* current, const server_id_list& to_update, bool master = false)
{
    try
    {
        gi_.set_database( opts_->geoip_database );
    }
    catch (std::exception& e)
    {
        statusBar()->showMessage(to_qstr(e.what()), 2000);
    }

    if ( master )
        que_->add_job( job_p( new job_update_from_master( current->server_list(), gi_, &(opts_->qstat_opts)) ) );
    else
        que_->add_job( job_p( new job_update_selected( to_update, current->server_list(), gi_, &(opts_->qstat_opts)) ) );
}


void main_window::refresh_all_bookmarks()
{
    server_list_widget* list = current_list_widget();
    assert( list == fav_list_ );
    server_id_list ids;
    foreach (const server_bookmark& bm, bookmarks_->list())
        ids.append(bm.id);
    refresh_servers( list, ids );
}

void main_window::refresh_selected()
{
    server_list_widget* list = current_list_widget();
    refresh_servers( list, list->selection() );
}

void main_window::refresh_master()
{
    server_list_widget* list = current_list_widget();
    assert( list == all_list_ );
    refresh_servers( list, server_id_list(), true );
}

void main_window::show_about()
{
    about_dialog d;
    d.exec();
}

void main_window::about_qt()
{
    QApplication::aboutQt();
}

server_id main_window::selected() const
{
    server_list_widget* list = current_list_widget();
    if (!list) return server_id();
    server_id_list sel = list->selection();
    if (sel.size() == 0) return server_id();
    return sel.front();
}

server_info_p main_window::selected_info() const
{
    const server_id& id = selected();
    if (id.is_empty())
        return server_info_p();
    else
        return all_sl_->get(id);
}

void main_window::connect_to_server(const server_id& id, const QString& player_name, const QString& password)
{
    check_anticheat_prereq();

    // update server info
    server_id_list idl;
    idl.push_back(id);

    job_p job(new job_update_selected(idl, all_sl_, gi_, &opts_->qstat_opts,
                                      tr("Update server info and launch a game")));
    que_->add_job(job);
    job->wait_for_finish();
    if (job->is_canceled())
        return;

    // info MUST be correct or connect-action is disabled!
    server_info_p info = all_sl_->get(id);

    const server_bookmark& opts = bookmarks_->get(info->id);

    launcher* l = launcher_;
    l->set_server_id(id);
    l->set_user_name(ui_->qlPlayerEdit->text());
    l->set_password(opts.password);

    if ( opts.password.isEmpty() && info && info->get_info("g_needpass").toInt() )
    {
        bool ok;
        QString password = QInputDialog::getText(0, tr("Server require password"), tr("Enter password"), QLineEdit::Normal, "", &ok );
        if ( !ok ) return;
        l->set_password(password);
    }

    if ( info && info->players.size() == info->max_player_count )
    {
        if (QMessageBox::warning( this
            , tr( "Server is full" )
            , tr( "Do you want to continue connecting?" )
            , QMessageBox::Yes | QMessageBox::No, QMessageBox::No
            ) != QMessageBox::Yes)
            return;
    }

    if ( info && info->players.size() == 0 )
    {
        if (QMessageBox::warning( this
            , tr( "Server is empty" )
            , tr( "Do you want to continue connecting?" )
            , QMessageBox::Yes | QMessageBox::No, QMessageBox::No
            ) != QMessageBox::Yes)
            return;
    }

    l->set_referee(opts.ref_password);
    l->set_rcon(opts.rcon_password);

    // add to history if history is enabled
    if (opts_->keep_history)
    {
        history_sl_->add(l->id(), info->name, l->user_name(), l->password());
        history_list_->update_history();
    }

    l->launch();
}

void main_window::connect_selected()
{
    server_id id = selected();
    if (id.is_empty())
        return;
    const QString& player_name = ui_->qlPlayerEdit->text();
    connect_to_server(id, player_name, QString());
}

server_list_widget* main_window::current_list_widget() const
{
    QWidget* curw = ui_->tabWidget->currentWidget();
    if (curw == ui_->tabFav)
        return fav_list_;
    else if (curw == ui_->tabAll)
        return all_list_;
    else
        return NULL;
}

void main_window::update_actions()
{
    server_list_widget* current = current_list_widget();
    bool sel = !(selected().is_empty());

    ui_->actionAddToFav->setEnabled(current == all_list_ && sel);
    ui_->actionConnect->setEnabled(sel);
    ui_->actionFavAdd->setEnabled(current == fav_list_);
    ui_->actionFavDelete->setEnabled(current == fav_list_ && sel);
    ui_->actionFavEdit->setEnabled(current == fav_list_ && sel);
    ui_->actionRefreshSelected->setEnabled(sel);

    ui_->actionClearSelected->setVisible(current);
    ui_->actionClearOffline->setVisible(current);
    ui_->actionClearAll->setVisible(current);
    ui_->actionOpenRemoteConsole->setEnabled(current == fav_list_ && sel);
    
    if ( current )
    {
        bool has_any_server = !( current->server_list()->list().empty() );
        ui_->actionClearSelected->setEnabled(sel);
        ui_->actionClearOffline->setEnabled(has_any_server);
        ui_->actionClearAll->setEnabled(has_any_server);
    }

    ui_->actionRefreshAll->setVisible(current == fav_list_);
    ui_->actionRefreshMaster->setVisible(current != fav_list_);
}

void main_window::current_tab_changed(int)
{
    update_actions();
}

void main_window::save_geometry()
{
    state_settings state_s;
    state_s.set_geometry( saveGeometry() );

    qsettings_p s = base_settings::get_settings( state_settings::uid() );
    s->setValue("window_state", saveState());
    s->setValue("fav_list_state", fav_list_->tree()->header()->saveState());
    s->setValue("all_list_state", all_list_->tree()->header()->saveState());
}

void main_window::load_geometry()
{
    qsettings_p s = base_settings::get_settings( state_settings::uid() );
                
    state_settings state_s;
    restoreGeometry( state_s.geometry() );

    qApp->processEvents();
    restoreState(s->value("window_state").toByteArray());
    fav_list_->tree()->header()->restoreState(s->value("fav_list_state").toByteArray());
    all_list_->tree()->header()->restoreState(s->value("all_list_state").toByteArray());
}

void main_window::update_server_info()
{
    update_tabs();
    
    if (!ui_->server_info_dock->isVisible())
        return;

    server_info_p si = selected_info();
    if (si)
    {
        if (old_id_ == si->id && old_state_ == si->update_stamp)
            return;

        old_state_ = si->update_stamp;
        old_id_ = si->id;

        QString s = get_server_info_html(*si);
        ui_->server_info_browser->setHtml(s);
    }
    else
    {
        old_id_ = server_id();
        old_state_ = 0;
//        ui_->server_info_browser->clear();
        ui_->server_info_browser->setHtml("");
    }
}

void main_window::selection_changed()
{
    update_actions();
    update_server_info();
}

void main_window::add_selected_to_fav()
{
    server_info_p si = selected_info();
    if (!si) return;

    server_bookmark opts;
    opts.id = si->id;
    opts.name = si->name;

    server_options_dialog d(this, opts);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    if (d.exec() == QDialog::Rejected) return;

    bookmarks_->add(d.options());
}

void main_window::show_action()
{
    setVisible(!isVisible());
}

// void save_list2(server_list_p list, const QString& name)
// {
//     QByteArray ba = save_server_list2(*(list.get()));
//     QString fn = get_server_list_settings(name)->fileName();
//     QFile f(fn);
//     f.open(QIODevice::WriteOnly);
//     f.write(ba);
// }

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
    hide();
    tray_->hide();
    save_state_at_exit();
    qApp->quit();
    launcher_->stop();
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

void main_window::update_tabs()
{
    QString s;
    int cnt1 = fav_list_->visible_server_count();
    int cnt2 = bookmarks_->list().size();
    if (cnt1 == cnt2)
        s = QString("%1").arg(cnt1);
    else
        s = QString("%1/%2").arg(cnt1).arg(cnt2);
    ui_->tabWidget->setTabText(0, tr("Favorites (%1)").arg(s));

    cnt1 = all_list_->visible_server_count();
    cnt2 = all_sl_->list().size();
    if (cnt1 == cnt2)
        s = QString("%1").arg(cnt1);
    else
        s = QString("%1/%2").arg(cnt1).arg(cnt2);
    ui_->tabWidget->setTabText(1, tr("All (%1)").arg(s));
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
    server_id_list id_list( current_list_widget()->selection() );

    if( !id_list.size() ) return;

    const server_id& id = selected();

    QDockWidget* dw = new QDockWidget( tr("RCon : %1").arg(id.address()), this );
    dw->setAttribute( Qt::WA_DeleteOnClose  );
    dw->setWidget( new rcon(this, id, bookmarks_->get(id)));

#if (QT_VERSION >= QT_VERSION_CHECK(4, 6, 0))
    dw->setStyle( new iconned_dock_style( QIcon(":/icons/icons/utilities-terminal.png"), dw->style() ) );
#endif
    addDockWidget(Qt::BottomDockWidgetArea,  dw );
}

void main_window::load_history_tab()
{
    if (opts_->keep_history)
    {
        if (!ui_->tabWidget->isTabEnabled(2))
        {
            ui_->tabWidget->setTabEnabled(2, true);
        }
        history_list_ = new history_widget(opts_, ui_->tabHistory, history_sl_);
        dynamic_cast<QBoxLayout*> (ui_->tabHistory->layout())->insertWidget(0, history_list_);
        connect(history_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

        history_list_->update_history();

        history_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
        history_list_->tree()->addAction(ui_->actionConnect);
        add_separator_action(history_list_->tree());
        history_list_->tree()->addAction(ui_->actionAddToFav);
        history_list_->tree()->addAction(ui_->actionHistoryDelete);
        add_separator_action(history_list_->tree());
        history_list_->tree()->addAction(ui_->actionRefreshSelected);

        new item_view_dblclick_action_link(this, history_list_->tree(), ui_->actionConnect);
    }
    else
    {
        QBoxLayout* tab_history_lay = dynamic_cast<QBoxLayout*> (ui_->tabHistory->layout());
        if (tab_history_lay->indexOf(history_list_) != -1)
        {
            tab_history_lay->removeWidget(history_list_);
            delete history_list_;
        }
        ui_->tabWidget->setTabEnabled(2, false);
    }
}

void main_window::launcher_started()
{
    if (!anticheat_enabled_action_->isChecked())
        return;

    QString player_name = ui_->qlPlayerEdit->text();

    anticheat_ = anticheat::create_anticheat(player_name, this);
    anticheat_->start();
}

void main_window::launcher_stopped()
{
    delete anticheat_;
}

void main_window::check_anticheat_prereq() const
{
    if (!anticheat_enabled_action_->isChecked())
        return;
    QString player_name = ui_->qlPlayerEdit->text();
    if (player_name.isEmpty())
        throw qexception(tr("A player's name must be defined in the quick launch window for anti-cheat!"));
}

