#include <iostream>
#include <set>

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

#include <cl/syslog/syslog.h>

#include "config.h"
#include "ui_main_window.h"
#include "options_dialog.h"
#include "launcher.h"
#include "exception.h"
#include "server_options_dialog.h"
#include "push_button_action_link.h"
#include "server_list.h"
#include "about_dialog.h"
#include "app_options_saver.h"
#include "server_list_saver.h"
#include "server_info_html.h"
#include "item_view_dblclick_action_link.h"
#include "str_convert.h"
#include "tools.h"

#include "jobs/job_monitor.h"
#include "job_update_selected.h"
#include "job_update_from_master.h"

#include "main_window.h"

SYSLOG_MODULE("main_window");

using namespace std;

////////////////////////////////////////////////////////////////////////////////
// main_window

main_window::main_window(QWidget *parent)
: QMainWindow(parent)
, ui_(new Ui::MainWindowClass)
, opts_(new app_options())
, all_sl_(new server_list)
, fav_sl_(new server_list)
, old_state_(0)
, clipper_( new clipper(this, opts_) )
{
    ui_->setupUi(this);

    que_ = new job_queue(this);
    job_monitor* jm = new job_monitor(que_, this);
    ui_->status_bar->addPermanentWidget(jm);

//#if (QT_VERSION >= QT_VERSION_CHECK(4, 5, 0))
//    ui_.tabWidget->setDocumentMode(true);
//#endif

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

    all_list_ = new server_list_widget(ui_->tabAll);
    QBoxLayout* tab_all_lay = dynamic_cast<QBoxLayout*> (ui_->tabAll->layout());
    tab_all_lay->insertWidget(0, all_list_);
    connect(all_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    fav_list_ = new server_list_widget(ui_->tabFav);
    dynamic_cast<QBoxLayout*> (ui_->tabFav->layout())->insertWidget(0, fav_list_);
    connect(fav_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    connect(ui_->tabWidget, SIGNAL(currentChanged(int)), SLOT(current_tab_changed(int)));
    connect(ui_->actionOptions, SIGNAL(triggered()), SLOT(show_options()));
    connect(ui_->actionQuickConnect, SIGNAL(triggered()), SLOT(quick_connect()));
    connect(ui_->actionFavAdd, SIGNAL(triggered()), SLOT(fav_add()));
    connect(ui_->actionFavEdit, SIGNAL(triggered()), SLOT(fav_edit()));
    connect(ui_->actionFavDelete, SIGNAL(triggered()), SLOT(fav_delete()));
    connect(ui_->actionRefreshSelected, SIGNAL(triggered()), SLOT(refresh_selected()));
    connect(ui_->actionRefreshAll, SIGNAL(triggered()), SLOT(refresh_all()));
    connect(ui_->actionAbout, SIGNAL(triggered()), SLOT(show_about()));
    connect(ui_->actionConnect, SIGNAL(triggered()), SLOT(connect_selected()));
    connect(ui_->actionAddToFav, SIGNAL(triggered()), SLOT(add_selected_to_fav()));
    connect(ui_->actionQuit, SIGNAL(triggered()), SLOT(quit_action()));
    connect(ui_->actionShow, SIGNAL(triggered()), SLOT(show_action()));

    connect(clipper_, SIGNAL(info_obtained()), SLOT(clipboard_info_obtained()));
    connect(qApp, SIGNAL(commitDataRequest(QSessionManager&)), SLOT(commit_data_request(QSessionManager&)));

    new push_button_action_link(this, ui_->quickConnectButton, ui_->actionQuickConnect);

    tab_size_updater* all_updater = new tab_size_updater( ui_->tabWidget,  ui_->tabWidget->indexOf( ui_->tabAll ) );
    connect(all_list_, SIGNAL(size_changed(int)), all_updater, SLOT(update_size(int)));
    
    all_list_->set_server_list(all_sl_);
    all_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    all_list_->tree()->addAction(ui_->actionConnect);
    add_separator_action(all_list_->tree());
    all_list_->tree()->addAction(ui_->actionAddToFav);
    add_separator_action(all_list_->tree());
    all_list_->tree()->addAction(ui_->actionRefreshAll);
    all_list_->tree()->addAction(ui_->actionRefreshSelected);
    
    new item_view_dblclick_action_link(this, all_list_->tree(), ui_->actionConnect);

    tab_size_updater* fav_updater = new tab_size_updater( ui_->tabWidget,  ui_->tabWidget->indexOf( ui_->tabFav ) );
    connect(fav_list_, SIGNAL(size_changed(int)), fav_updater, SLOT(update_size(int)));

    fav_list_->set_server_list(fav_sl_);
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

    fav_list_->set_favs(&(opts_->servers));
    all_list_->set_favs(&(opts_->servers));

    // loading all options
    load_all_at_start();
    load_geometry();

    update_actions();
    sync_fav_list();
    update_server_info();
    setVisible(!(opts_->start_hidden));
    all_list_->force_update();
    fav_list_->force_update();
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
    options_dialog d;
    d.set_opts(opts_);
    if (d.exec() == QDialog::Rejected) return;
    save_options();
}

void main_window::quick_connect() const
{
    launcher l( opts_ );
    l.set_server_id(server_id(ui_->qlServerEdit->text()));
    l.set_user_name(ui_->qlPlayerEdit->text());
    l.set_password(ui_->qlPasswordEdit->text());
    l.launch();
}

void main_window::fav_add()
{
    server_options_dialog d;
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    sync_fav_list();
    fav_list_->force_update();
    update_actions();
    save_favorites();
}

void main_window::fav_edit()
{
    server_id id = selected();
    if (id.is_empty()) return;
    server_options opts = opts_->servers[id];

    server_options_dialog d(this, opts);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);
    if (d.exec() == QDialog::Rejected) return;

    if (d.options().id != id)
    {
        opts_->servers.erase(id);
        id = d.options().id;
    }

    opts_->servers[id] = d.options();
    save_favorites();
    sync_fav_list();
    fav_list_->force_update();

    update_actions();
}

void main_window::fav_delete()
{
    if (QMessageBox::question(this, tr("Delete a favorite"),
                              tr("Continue to delete a favorite"),
                              QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;

    server_id_list sel = fav_list_->selection();
    server_fav_list& list = opts_->servers;

    for (server_id_list::iterator it = sel.begin(); it != sel.end(); it++)
        list.erase(*it);

    sync_fav_list();
    fav_list_->force_update();
    update_actions();
    save_favorites();
}

void main_window::sync_fav_list()
{
    server_fav_list& srclist = opts_->servers;
    server_info_list& dstlist = fav_sl_->list();

    // remember all server id's for remove unused later
    typedef std::set<server_id> id_set;
    id_set ids;
    for (server_info_list::iterator it = dstlist.begin(); it != dstlist.end(); it++)
        ids.insert(it->first);

    // adding new items and remember that server id in use
    bool changed = false;
    for (server_fav_list::iterator it = srclist.begin(); it != srclist.end(); it++)
    {
        const server_id& id = it->first;
        ids.erase(id);
        server_info_list::iterator it2 = dstlist.find(id);
        if (it2 == dstlist.end())
        {
            server_options& opts = it->second;

            server_info_p si( new server_info );
            si->id = id;
//             si->name = opts.name;
            dstlist[id] = si;
            changed = true;
        }
    }

    // remove unused
    if (!ids.empty())
    {
        changed = true;
        for (id_set::iterator it = ids.begin(); it != ids.end(); it++)
            dstlist.erase(*it);
    }

    if (changed)
        fav_sl_->change_state();
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
    load_server_favs(get_app_options_settings("favorites"), opts_);

    local::load_list(all_sl_, "all_state");
    local::load_list(fav_sl_, "favs_state");
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
    local::save_list(fav_sl_, "favs_state");
}

void main_window::save_favorites()
{
    save_server_favs(get_app_options_settings("favorites"), opts_);
}

void main_window::refresh_all()
{
    try {
        gi_.set_database( opts_->geoip_database );
    } catch (std::exception& e) {
        statusBar()->showMessage(to_qstr(e.what()), 2000);
    }

    server_list_widget* list = selected_list_widget();
    if (!list) return;
    server_list_p sl = list->server_list();
    if (list == all_list_)
    {
        que_->add_job(job_p(new job_update_from_master(list->server_list(),
            gi_, &(opts_->qstat_opts))));
    } else
    {
        server_fav_list& fav_list = opts_->servers;
        server_id_list ids;
        for (server_fav_list::iterator it = fav_list.begin(); it != fav_list.end(); it++)
            ids.push_back(it->first);
        que_->add_job(job_p(new job_update_selected(ids, list->server_list(), gi_, &(opts_->qstat_opts))));
    }
}

void main_window::refresh_selected()
{
    try 
    {
        gi_.set_database( opts_->geoip_database );
    } 
    catch (std::exception& e) 
    {
        statusBar()->showMessage(to_qstr(e.what()), 2000);
    }
    
    server_list_widget* list = selected_list_widget();
    if (!list) return;
    server_id_list sel = list->selection();
    if (sel.empty()) return;
    que_->add_job(job_p(new job_update_selected(sel, list->server_list(), gi_, &(opts_->qstat_opts))));
}

void main_window::show_about()
{
    about_dialog d;
    d.exec();
}

server_id main_window::selected() const
{
    server_list_widget* list = selected_list_widget();
    if (!list) return server_id();
    server_id_list sel = list->selection();
    if (sel.size() == 0) return server_id();
    return sel.front();
}

server_info_p main_window::selected_info() const
{
    server_list_widget* list = selected_list_widget();
    if (!list)
        return server_info_p();
    server_id id = selected();
    if (id.is_empty())
        return server_info_p();
    const server_info_list& sil = list->server_list()->list();

    server_info_list::const_iterator it = sil.find(id);
    if (it == sil.end())
        return server_info_p();

    return it->second;
}

void main_window::connect_selected() const
{
    //info MUST be correct or connect-action is disabled!
    server_info_p info = selected_info();

    server_options opts;

    server_fav_list::const_iterator it;

    //may be wrap server_info in shared_ptr?
    if ( ( it = opts_->servers.find( info->id ) ) != opts_->servers.end() )
        opts = it->second;

    //i think launcher can be created on stack
    launcher l(opts_);

    l.set_server_id( info->id );
    l.set_user_name("");
    l.set_password(opts.password);

    if ( opts.password.isEmpty() && selected_info() && selected_info()->get_info("g_needpass").toInt() )
    {
        bool ok;
        QString password = QInputDialog::getText(0, "Server require password", "Enter password:", QLineEdit::PasswordEchoOnEdit, "", &ok );
        if ( ok ) return;
        l.set_password(password);
    }

    l.set_referee(opts.ref_password);
    l.set_rcon(opts.rcon_password);
    l.launch();
}

server_list_widget* main_window::selected_list_widget() const
{
    QWidget* curw = ui_->tabWidget->currentWidget();
    if (curw == ui_->tabFav)
        return fav_list_;
    else if (curw == ui_->tabAll)
        return all_list_;
    else return 0;
}

void main_window::update_actions()
{
    QWidget* curw = selected_list_widget();
    bool sel = !(selected().is_empty());

    ui_->actionAddToFav->setEnabled(curw == all_list_ && sel);
    ui_->actionConnect->setEnabled(sel);
    ui_->actionFavAdd->setEnabled(curw == fav_list_);
    ui_->actionFavDelete->setEnabled(curw == fav_list_ && sel);
    ui_->actionFavEdit->setEnabled(curw == fav_list_ && sel);
    ui_->actionRefreshSelected->setEnabled(sel);
}

void main_window::current_tab_changed(int)
{
    update_actions();
}

void main_window::save_geometry()
{
    qsettings_p s = get_app_options_settings("state");
    s->setValue("geometry", saveGeometry());
    s->setValue("window_state", saveState());
    s->setValue("fav_list_state", fav_list_->tree()->header()->saveState());
    s->setValue("all_list_state", all_list_->tree()->header()->saveState());
}

void main_window::load_geometry()
{
    qsettings_p s = get_app_options_settings("state");
    restoreGeometry(s->value("geometry").toByteArray());
    qApp->processEvents();
    restoreState(s->value("window_state").toByteArray());
    fav_list_->tree()->header()->restoreState(s->value("fav_list_state").toByteArray());
    all_list_->tree()->header()->restoreState(s->value("all_list_state").toByteArray());
}

void main_window::update_server_info()
{
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
        ui_->server_info_browser->clear();
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

    server_options opts;
    opts.id = si->id;
    opts.name = si->name;

    server_options_dialog d(0, opts);
    d.set_update_params(&gi_, &(opts_->qstat_opts), que_);

    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    save_options();
    sync_fav_list();
    fav_list_->force_update();
    update_actions();
    save_favorites();
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
    LOG_DEBUG << "Quit action";
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
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        ui_->actionQuickConnect->trigger();
    }
}

////////////////////////////////////////////////////////////////////////////////
// tab_size_updater

tab_size_updater::tab_size_updater(QTabWidget* tw, int index)
        : QObject(tw)
        , tw_(tw)
        , index_(index)
{}

tab_size_updater::~tab_size_updater()
{}

void tab_size_updater::update_size(int size) const
{
    QString tabtext = tw_->tabText( index_ );
    static QRegExp rx("([^()]+).*");
    rx.exactMatch(tabtext);
    QString new_text("%1(%2)");
    tw_->setTabText( index_, QString("%1(%2)").arg(rx.cap(1)).arg(size) );
}


