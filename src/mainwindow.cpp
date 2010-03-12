#include <iostream>

#include <QAction>
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

using namespace std;

main_window::main_window(QWidget *parent)
 : QMainWindow(parent),
   opts_( new app_options() ),
   launcher_(opts_)
{
    ui.setupUi(this);

    all_list_ = new serv_list_widget(ui.tabAll);
    dynamic_cast<QBoxLayout*>(ui.tabAll->layout())->insertWidget(0, all_list_);
    connect(all_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(update_actions()));

    fav_list_ = new serv_list_widget(ui.tabFav);
    dynamic_cast<QBoxLayout*>(ui.tabFav->layout())->insertWidget(0, fav_list_);
    connect(fav_list_->tree(), SIGNAL(itemSelectionChanged()), SLOT(update_actions()));

    connect(ui.tabWidget, SIGNAL(currentChanged(int)), SLOT(current_tab_changed(int)));
    connect(ui.actionOptions, SIGNAL( triggered() ), SLOT( show_options() ) );
    connect(ui.actionQuickConnect, SIGNAL( triggered() ), SLOT( quick_connect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launch_status_changed()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launch_status_changed()));
    connect(ui.actionFavAdd, SIGNAL(triggered()), SLOT(fav_add()));
    connect(ui.actionFavEdit, SIGNAL(triggered()), SLOT(fav_edit()));
    connect(ui.actionFavDelete, SIGNAL(triggered()), SLOT(fav_delete()));
    connect(ui.actionRefreshSelected, SIGNAL(triggered()), SLOT(refresh_selected()));
    connect(ui.actionRefreshAll, SIGNAL(triggered()), SLOT(refresh_all()));
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(show_about()));
    connect(ui.actionConnect, SIGNAL(triggered()), SLOT(connect_selected()));


//    new PushButtonActionLink(ui.favAddButton, ui.actionFavAdd);
//    new PushButtonActionLink(ui.favDeleteButton, ui.actionFavDelete);
    new push_button_action_link(this, ui.quickConnectButton, ui.actionQuickConnect);
//    new PushButtonActionLink(ui.favConnectButton, ui.actionConnectToFavorite);
//    new PushButtonActionLink(ui.refreshAllButton, ui.actionRefreshAll);

    load_options();
    load_geometry();

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
    all_list_->tree()->addAction(ui.actionConnect);
    all_list_->tree()->addAction(ui.actionAddToFav);
    all_list_->tree()->addAction(ui.actionRefreshAll);
    all_list_->tree()->addAction(ui.actionRefreshSelected);
    
    fav_list_->setServerList(fav_sl_);
    fav_list_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    fav_list_->tree()->addAction(ui.actionConnect);
    fav_list_->tree()->addAction(ui.actionFavAdd);
    fav_list_->tree()->addAction(ui.actionFavEdit);
    fav_list_->tree()->addAction(ui.actionFavDelete);
    fav_list_->tree()->addAction(ui.actionRefreshSelected);
    fav_list_->tree()->addAction(ui.actionRefreshAll);

    update_actions();
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
    launcher_.set_server_id( server_id( ui.qlServerEdit->text() ) );
    launcher_.set_user_name( ui.qlPlayerEdit->text() );
    launcher_.set_password( ui.qlPasswordEdit->text() );
    launcher_.launch();
}

void main_window::launch_status_changed()
{
    cout << launcher_.executing() << endl;
    ui.quickConnectButton->setEnabled( !launcher_.executing() );
}

void main_window::fav_add()
{
    ServOptsDialog d;
    if (d.exec() == QDialog::Rejected) return;
    ServerOptionsList& list = opts_->servers;
    list[d.options().id] = d.options();
    save_options();
    sync_fav_list();
    fav_list_->forceUpdate();
    update_actions();
}

void main_window::fav_delete()
{
    if (QMessageBox::question(this, tr("Delete a favorite"),
            tr("Continue to delete a favorite"), QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    ServerIDList sel = fav_list_->selection();
    ServerOptionsList& list = opts_->servers;
    for (ServerIDList::iterator it = sel.begin(); it != sel.end(); it++)
        list.erase(*it);
    save_options();
    sync_fav_list();
    fav_list_->forceUpdate();
    update_actions();
}

void main_window::sync_fav_list()
{
    ServerOptionsList& srclist = opts_->servers;
    ServerIDList& dstlist = fav_sl_->customServList();
    dstlist.clear();

    for (ServerOptionsList::iterator it = srclist.begin(); it != srclist.end(); it++)
        dstlist.push_back((*it).second.id);
    fav_sl_->update();
}

void main_window::save_options()
{
    save_app_options(*opts_);
}

void main_window::load_options()
{
    opts_->qstat_opts.qstat_path = "/usr/bin/qstat";
    opts_->qstat_opts.master_server = "master.urbanterror.net";

    load_app_options(*opts_);
}

void main_window::refresh_all()
{
    serv_list_widget* list = selected_list_widget();
    if (!list) return;
    serv_list_custom* sl = list->serverList();
    sl->refreshAll();
    ui.actionRefreshAll->setEnabled(false);
}

void main_window::refresh_all_stopped()
{
    ui.actionRefreshAll->setEnabled(true);
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

void main_window::connect_selected()
{
    server_id id = selected();
    if (id.isEmpty()) return;

    ServerOptions& opts = opts_->servers[id];

    launcher_.set_server_id( id );
    launcher_.set_user_name("");
    launcher_.set_password(opts.password);
    launcher_.set_referee(opts.refPassword);
    launcher_.set_rcon(opts.rconPassword);
    launcher_.launch();
}

serv_list_widget* main_window::selected_list_widget()
{
    QWidget* curw = ui.tabWidget->currentWidget();
    if (curw == ui.tabFav)
        return fav_list_;
    else if (curw == ui.tabAll)
        return all_list_;
    else return 0;
}

void main_window::update_actions()
{
    QWidget* curw = selected_list_widget();
    bool sel = !(selected().isEmpty());

    ui.actionAddToFav->setEnabled(curw == all_list_ && sel);
    ui.actionConnect->setEnabled(sel);
    ui.actionFavAdd->setEnabled(curw == fav_list_);
    ui.actionFavDelete->setEnabled(curw == fav_list_ && sel);
    ui.actionFavEdit->setEnabled(curw == fav_list_ && sel);
    ui.actionRefreshSelected->setEnabled(sel);
}

void main_window::fav_edit()
{
    server_id id = selected();
    if (id.isEmpty()) return;
    ServerOptions opts = opts_->servers[id];

    ServOptsDialog d(this, opts);
    if (d.exec() == QDialog::Rejected) return;

    opts_->servers[id] = d.options();
    save_options();
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
    QSettings s;
    s.setValue("geometry", saveGeometry());
    s.setValue("windowState", saveState());
}

void main_window::load_geometry()
{
    QSettings s;
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("windowState").toByteArray());
}
