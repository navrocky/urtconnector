#include <iostream>

#include <QAction>
#include <QTimer>
#include <QSettings>
#include <QMenu>
#include <QTreeView>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QMessageBox>
#include <qobjectdefs.h>

#include "mainwindow.h"
#include "optionsdialog.h"
#include "launcher.h"
#include "exception.h"
#include "servoptsdialog.h"
#include "pushbuttonactionlink.h"
#include "serverlistqstat.h"
#include "aboutdialog.h"
#include "app_options_saver.h"
#include "server_info_html.h"

using namespace std;

main_window::main_window(QWidget *parent)
 : QMainWindow(parent),
   opts_( new app_options() ),
   launcher_(opts_)
{
    ui_.setupUi(this);

    serv_info_update_timer_ = new QTimer(this);
    serv_info_update_timer_->setInterval(1000);
    connect(serv_info_update_timer_, SIGNAL(timeout()), SLOT(update_server_info()));
    serv_info_update_timer_->start();

    all_list_ = new serv_list_widget(ui_.tabAll);
    dynamic_cast<QBoxLayout*>(ui_.tabAll->layout())->insertWidget(0, all_list_);
    connect(all_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    fav_list_ = new serv_list_widget(ui_.tabFav);
    dynamic_cast<QBoxLayout*>(ui_.tabFav->layout())->insertWidget(0, fav_list_);
    connect(fav_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(selection_changed()));

    connect(ui_.tabWidget, SIGNAL(currentChanged(int)), SLOT(current_tab_changed(int)));
    connect(ui_.actionOptions, SIGNAL( triggered() ), SLOT( show_options() ) );
    connect(ui_.actionQuickConnect, SIGNAL( triggered() ), SLOT( quick_connect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launch_status_changed()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launch_status_changed()));
    connect(ui_.actionFavAdd, SIGNAL(triggered()), SLOT(fav_add()));
    connect(ui_.actionFavEdit, SIGNAL(triggered()), SLOT(fav_edit()));
    connect(ui_.actionFavDelete, SIGNAL(triggered()), SLOT(fav_delete()));
    connect(ui_.actionRefreshSelected, SIGNAL(triggered()), SLOT(refresh_selected()));
    connect(ui_.actionRefreshAll, SIGNAL(triggered()), SLOT(refresh_all()));
    connect(ui_.actionAbout, SIGNAL(triggered()), SLOT(show_about()));
    connect(ui_.actionConnect, SIGNAL(triggered()), SLOT(connect_selected()));
    connect(ui_.actionAddToFav, SIGNAL(triggered()), SLOT(add_selected_to_fav()));


//    new PushButtonActionLink(ui.favAddButton, ui.actionFavAdd);
//    new PushButtonActionLink(ui.favDeleteButton, ui.actionFavDelete);
    new push_button_action_link(this, ui_.quickConnectButton, ui_.actionQuickConnect);
//    new PushButtonActionLink(ui.favConnectButton, ui.actionConnectToFavorite);
//    new PushButtonActionLink(ui.refreshAllButton, ui.actionRefreshAll);

    load_options();
    load_geometry();
    load_server_favs(*opts_);

    ServerListQStat* allSL = new ServerListQStat(this);
    all_sl_ = allSL;
    allSL->setOpts(&(opts_->servers));
    allSL->setQStatOpts(&(opts_->qstat_opts));

    ServerListQStat* favSL = new ServerListQStat(this);
    fav_sl_ = favSL;
    favSL->setOpts(&(opts_->servers));
    favSL->setQStatOpts(&(opts_->qstat_opts));

    all_list_->setServerList(all_sl_);
    connect(all_sl_, SIGNAL(refreshStopped()), SLOT(refresh_all_stopped()));
    all_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    all_list_->tree()->addAction(ui_.actionConnect);
    all_list_->tree()->addAction(ui_.actionAddToFav);
    all_list_->tree()->addAction(ui_.actionRefreshAll);
    all_list_->tree()->addAction(ui_.actionRefreshSelected);
    
    fav_list_->setServerList(fav_sl_);
    connect(fav_sl_, SIGNAL(refreshStopped()), SLOT(refresh_all_stopped()));
    fav_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    fav_list_->tree()->addAction(ui_.actionConnect);
    fav_list_->tree()->addAction(ui_.actionFavAdd);
    fav_list_->tree()->addAction(ui_.actionFavEdit);
    fav_list_->tree()->addAction(ui_.actionFavDelete);
    fav_list_->tree()->addAction(ui_.actionRefreshSelected);
    fav_list_->tree()->addAction(ui_.actionRefreshAll);

    update_actions();
    sync_fav_list();
    update_server_info();
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
    launcher_.set_server_id( server_id( ui_.qlServerEdit->text() ) );
    launcher_.set_user_name( ui_.qlPlayerEdit->text() );
    launcher_.set_password( ui_.qlPasswordEdit->text() );
    launcher_.launch();
}

void main_window::launch_status_changed()
{
    cout << launcher_.executing() << endl;
    ui_.quickConnectButton->setEnabled( !launcher_.executing() );
}

void main_window::fav_add()
{
    ServOptsDialog d;
    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    save_options();
    sync_fav_list();
    fav_list_->forceUpdate();
    update_actions();
    save_server_favs(*opts_);
}

void main_window::fav_delete()
{
    if (QMessageBox::question(this, tr("Delete a favorite"),
            tr("Continue to delete a favorite"), QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    ServerIDList sel = fav_list_->selection();
    server_fav_list& list = opts_->servers;
    for (ServerIDList::iterator it = sel.begin(); it != sel.end(); it++)
        list.erase(*it);
    save_options();
    sync_fav_list();
    fav_list_->forceUpdate();
    update_actions();
}

void main_window::sync_fav_list()
{
    server_fav_list& srclist = opts_->servers;
    ServerIDList& dstlist = fav_sl_->customServList();
    dstlist.clear();

    for (server_fav_list::iterator it = srclist.begin(); it != srclist.end(); it++)
        dstlist.push_back((*it).second.id);
    fav_sl_->update();
}

void main_window::save_options()
{
    save_app_options(*opts_);
}

void main_window::load_options()
{
#if defined(Q_OS_WIN)
    opts_->qstat_opts.qstat_path = "qstat\qstat.exe";
#elif defined(Q_OS_UNIX)
    opts_->qstat_opts.qstat_path = "/usr/bin/qstat";
#endif
    
    opts_->qstat_opts.master_server = "master.urbanterror.net";

    load_app_options(*opts_);
}

void main_window::refresh_all()
{
    serv_list_widget* list = selected_list_widget();
    if (!list) return;
    serv_list_custom* sl = list->serverList();
    sl->refreshAll();
    ui_.actionRefreshAll->setEnabled(false);
}

void main_window::refresh_all_stopped()
{
    ui_.actionRefreshAll->setEnabled(true);
}

void main_window::show_about()
{
    about_dialog d;
    d.exec();
}

void main_window::refresh_selected()
{
    server_id id = selected();
    if (id.isEmpty()) return;
    serv_list_widget* list = selected_list_widget();
    if (!list) return;
    list->serverList()->refreshServer(id);
}

server_id main_window::selected()
{
    serv_list_widget* list = selected_list_widget();
    if (!list) return server_id();
    ServerIDList sel = list->selection();
    if (sel.size() == 0) return server_id();
    return sel.front();
}

const server_info* main_window::selected_info()
{
    serv_list_widget* list = selected_list_widget();
    if (!list)
        return 0;
    server_id id = selected();
    if (id.isEmpty())
        return 0;
    const ServerInfoList& sil = list->serverList()->list();

    ServerInfoList::const_iterator it = sil.find(id);
    if (it == sil.end())
        return 0;
    const server_info& si = it->second;
    return &si;
}

void main_window::connect_selected()
{
    server_id id = selected();
    if (id.isEmpty()) return;

    server_options& opts = opts_->servers[id];

    launcher_.set_server_id( id );
    launcher_.set_user_name("");
    launcher_.set_password(opts.password);
    launcher_.set_referee(opts.ref_password);
    launcher_.set_rcon(opts.rcon_password);
    launcher_.launch();
}

serv_list_widget* main_window::selected_list_widget()
{
    QWidget* curw = ui_.tabWidget->currentWidget();
    if (curw == ui_.tabFav)
        return fav_list_;
    else if (curw == ui_.tabAll)
        return all_list_;
    else return 0;
}

void main_window::update_actions()
{
    QWidget* curw = selected_list_widget();
    bool sel = !(selected().isEmpty());

    ui_.actionAddToFav->setEnabled(curw == all_list_ && sel);
    ui_.actionConnect->setEnabled(sel);
    ui_.actionFavAdd->setEnabled(curw == fav_list_);
    ui_.actionFavDelete->setEnabled(curw == fav_list_ && sel);
    ui_.actionFavEdit->setEnabled(curw == fav_list_ && sel);
    ui_.actionRefreshSelected->setEnabled(sel);
}

void main_window::fav_edit()
{
    server_id id = selected();
    if (id.isEmpty()) return;
    server_options opts = opts_->servers[id];

    ServOptsDialog d(this, opts);
    if (d.exec() == QDialog::Rejected) return;

    opts_->servers[id] = d.options();
    save_server_favs(*opts_);
    sync_fav_list();
    fav_list_->forceUpdate();

    update_actions();
}

void main_window::current_tab_changed(int)
{
    update_actions();
}

void main_window::closeEvent(QCloseEvent *event)
{
    save_geometry();
    QMainWindow::closeEvent(event);
}

void main_window::save_geometry()
{
    qsettings_p s = get_app_options_settings();
    s->setValue("geometry", saveGeometry());
    s->setValue("window_state", saveState());
}

void main_window::load_geometry()
{
    qsettings_p s = get_app_options_settings();
    restoreGeometry(s->value("geometry").toByteArray());
    restoreState(s->value("window_state").toByteArray());
}

void main_window::update_server_info()
{
    const server_info* si = selected_info();
    QString html;
    if (si)
        html = get_server_info_html(*si);

    if (old_info_ != html)
    {
        ui_.server_info_browser->setHtml(html);
        old_info_ = html;
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

    ServOptsDialog d(0, opts);
    if (d.exec() == QDialog::Rejected) return;
    server_fav_list& list = opts_->servers;
    list[d.options().id] = d.options();
    save_options();
    sync_fav_list();
    fav_list_->forceUpdate();
    update_actions();
    save_server_favs(*opts_);
}
