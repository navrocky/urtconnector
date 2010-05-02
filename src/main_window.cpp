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
#include <QHeaderView>
#include <QInputDialog>

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

#include "jobs/job_monitor.h"
#include "job_update_selected.h"
#include "job_update_from_master.h"

#include "main_window.h"

using namespace std;

main_window::main_window(QWidget *parent)
: QMainWindow(parent)
, ui_(new Ui::MainWindowClass)
, opts_(new app_options())
, launcher_(opts_)
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

    tray_menu_ = new QMenu(this);
    tray_menu_->addAction(ui_->actionShow);
    tray_menu_->addSeparator();
    tray_menu_->addAction(ui_->actionQuit);

    tray_ = new QSystemTrayIcon(this);
    tray_->setIcon(QIcon(":/images/icons/logo.png"));
    tray_->show();
    tray_->setContextMenu(tray_menu_);

    connect(tray_, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(tray_activated(QSystemTrayIcon::ActivationReason)));

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

    connect(clipper_, SIGNAL(address_obtained( const QString& )), ui_->qlServerEdit, SLOT( setText( const QString& ) ) );

    new push_button_action_link(this, ui_->quickConnectButton, ui_->actionQuickConnect);

    qsettings_p s = get_app_options_settings();
    load_server_list(s, "all_list_info", *(all_sl_.get()));
    load_options();
    load_server_favs(*opts_);

    all_list_->set_server_list(all_sl_);
    all_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    all_list_->tree()->addAction(ui_->actionConnect);
    all_list_->tree()->addAction(ui_->actionAddToFav);
    all_list_->tree()->addAction(ui_->actionRefreshAll);
    all_list_->tree()->addAction(ui_->actionRefreshSelected);

    new item_view_dblclick_action_link(this, all_list_->tree(), ui_->actionConnect);

    fav_list_->set_server_list(fav_sl_);
    fav_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    fav_list_->tree()->addAction(ui_->actionConnect);
    fav_list_->tree()->addAction(ui_->actionFavAdd);
    fav_list_->tree()->addAction(ui_->actionFavEdit);
    fav_list_->tree()->addAction(ui_->actionFavDelete);
    fav_list_->tree()->addAction(ui_->actionRefreshSelected);
    fav_list_->tree()->addAction(ui_->actionRefreshAll);

    new item_view_dblclick_action_link(this, fav_list_->tree(), ui_->actionConnect);

    update_actions();
    sync_fav_list();
    update_server_info();
    load_geometry(s);
    setVisible(!(opts_->start_hidden));
    all_list_->force_update();
}

main_window::~main_window()
{
}

void main_window::show_options()
{
    options_dialog d;
    d.set_opts(opts_);
    if (d.exec() == QDialog::Rejected) return;
    save_options();
}

void main_window::quick_connect()
{
    launcher_.set_server_id(server_id(ui_->qlServerEdit->text()));
    launcher_.set_user_name(ui_->qlPlayerEdit->text());
    launcher_.set_password(ui_->qlPasswordEdit->text());
    launcher_.launch();
}

void main_window::fav_add()
{
    server_options_dialog d;
    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    sync_fav_list();
    fav_list_->force_update();
    update_actions();
    save_server_favs(*opts_);
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
    save_server_favs(*opts_);
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

            server_info si;
            si.id = id;
            si.name = opts.name;
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
    qsettings_p s = get_app_options_settings();
    save_app_options(s, *opts_);
}

void main_window::load_options()
{
#if defined(Q_OS_UNIX)
    opts_->qstat_opts.qstat_path = "/usr/bin/qstat";
    QString default_database = QString(URT_DATADIR) + "GeoIP.dat";
#elif defined(Q_OS_WIN)
    opts_->qstat_opts.qstat_path = "qstat.exe";
    QString default_database = QString(URT_DATADIR) + "GeoIP.dat";
#elif defined(Q_OS_MAC)
    QString default_database = QString(URT_DATADIR) + "GeoIP.dat";
#endif

    opts_->qstat_opts.master_server = "master.urbanterror.net";

    qsettings_p s = get_app_options_settings();
    load_app_options(s, *opts_);
}

void main_window::refresh_all()
{
    try{
        gi_.set_database( opts_->geoip_database );
    } catch (std::exception& e){
        statusBar()->showMessage (  to_qstr(e.what()) );
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

void main_window::show_about()
{
    about_dialog d;
    d.exec();
}

void main_window::refresh_selected()
{
    try{
        gi_.set_database( opts_->geoip_database );
    } catch (std::exception& e){
        statusBar()->showMessage (  to_qstr(e.what()) );
    }
    
    server_id id = selected();
    if (id.is_empty()) return;
    server_list_widget* list = selected_list_widget();
    if (!list) return;
    server_id_list ids;
    ids.push_back(id);
    que_->add_job(job_p(new job_update_selected(ids, list->server_list(), gi_, &(opts_->qstat_opts))));
}

server_id main_window::selected()
{
    server_list_widget* list = selected_list_widget();
    if (!list) return server_id();
    server_id_list sel = list->selection();
    if (sel.size() == 0) return server_id();
    return sel.front();
}

const server_info* main_window::selected_info()
{
    server_list_widget* list = selected_list_widget();
    if (!list)
        return 0;
    server_id id = selected();
    if (id.is_empty())
        return 0;
    const server_info_list& sil = list->server_list()->list();

    server_info_list::const_iterator it = sil.find(id);
    if (it == sil.end())
        return 0;
    const server_info& si = it->second;
    return &si;
}

void main_window::connect_selected()
{
    server_id id = selected();
    if (id.is_empty()) return;

    server_options& opts = opts_->servers[id];

    launcher_.set_server_id(id);
    launcher_.set_user_name("");
    launcher_.set_password(opts.password);
    
    if ( opts.password.isEmpty() && selected_info() && selected_info()->get_info("g_needpass").toInt() )
    {
        bool ok;
        QString password = QInputDialog::getText(this, "Server require password", "Enter password:", QLineEdit::PasswordEchoOnEdit, "", &ok );
        if ( !ok ) return;
        launcher_.set_password(password);
    }

    launcher_.set_referee(opts.ref_password);
    launcher_.set_rcon(opts.rcon_password);
    launcher_.launch();
}

server_list_widget* main_window::selected_list_widget()
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

void main_window::fav_edit()
{
    server_id id = selected();
    if (id.is_empty()) return;
    server_options opts = opts_->servers[id];

    server_options_dialog d(this, opts);
    if (d.exec() == QDialog::Rejected) return;

    opts_->servers[id] = d.options();
    save_server_favs(*opts_);
    sync_fav_list();
    fav_list_->force_update();

    update_actions();
}

void main_window::current_tab_changed(int)
{
    update_actions();
}

void main_window::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void main_window::save_geometry(qsettings_p s)
{
    s->setValue("geometry", saveGeometry());
    s->setValue("window_state", saveState());
    s->setValue("fav_list_state", fav_list_->tree()->header()->saveState());
    s->setValue("all_list_state", all_list_->tree()->header()->saveState());
}

void main_window::load_geometry(qsettings_p s)
{
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

    const server_info* si = selected_info();
    if (si)
    {
        if (old_id_ == si->id && old_state_ == si->update_stamp)
            return;

        old_state_ = si->update_stamp;
        old_id_ = si->id;

        ui_->server_info_browser->setHtml(get_server_info_html(*si));
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
    const server_info* si = selected_info();
    if (!si) return;

    server_options opts;
    opts.id = si->id;
    opts.name = si->name;

    server_options_dialog d(0, opts);
    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    save_options();
    sync_fav_list();
    fav_list_->force_update();
    update_actions();
    save_server_favs(*opts_);
}

void main_window::show_action()
{
    setVisible(!isVisible());
}

void main_window::quit_action()
{
    qsettings_p s = get_app_options_settings();
    save_geometry(s);
    save_server_list(s, "all_list_info", *(all_sl_.get()));


    qApp->quit();
}

void main_window::tray_activated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger)
    {
        ui_->actionShow->trigger();
    }
}