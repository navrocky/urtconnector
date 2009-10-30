#include <iostream>

#include <QAction>
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

using namespace std;

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   opts_( new AppOptions() ),
   launcher_(opts_)
{
    ui.setupUi(this);

    allList_ = new ServListWidget(ui.tabAll);
    dynamic_cast<QBoxLayout*>(ui.tabAll->layout())->insertWidget(0, allList_);
    connect(allList_->tree(), SIGNAL(itemSelectionChanged()), SLOT(updateActions()));

    favList_ = new ServListWidget(ui.tabFav);
    dynamic_cast<QBoxLayout*>(ui.tabFav->layout())->insertWidget(0, favList_);
    connect(favList_->tree(), SIGNAL(itemSelectionChanged()), SLOT(updateActions()));

    connect(ui.tabWidget, SIGNAL(currentChanged(int)), SLOT(currentTabChanged(int)));
    connect(ui.actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );
    connect(ui.actionQuickConnect, SIGNAL( triggered() ), SLOT( quickConnect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launchStatusChanged()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launchStatusChanged()));
    connect(ui.actionFavAdd, SIGNAL(triggered()), SLOT(favAdd()));
    connect(ui.actionFavEdit, SIGNAL(triggered()), SLOT(favEdit()));
    connect(ui.actionFavDelete, SIGNAL(triggered()), SLOT(favDelete()));
    connect(ui.actionRefreshSelected, SIGNAL(triggered()), SLOT(refreshSelected()));
    connect(ui.actionRefreshAll, SIGNAL(triggered()), SLOT(refreshAll()));
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(showAbout()));
    connect(ui.actionConnect, SIGNAL(triggered()), SLOT(connectSelected()));


//    new PushButtonActionLink(ui.favAddButton, ui.actionFavAdd);
//    new PushButtonActionLink(ui.favDeleteButton, ui.actionFavDelete);
    new PushButtonActionLink(ui.quickConnectButton, ui.actionQuickConnect);
//    new PushButtonActionLink(ui.favConnectButton, ui.actionConnectToFavorite);
//    new PushButtonActionLink(ui.refreshAllButton, ui.actionRefreshAll);

    loadOptions();

    ServerListQStat* allSL = new ServerListQStat(this);
    allSL_ = allSL;
    allSL->setOpts(&(opts_->servers));
    allSL->setQStatOpts(&(opts_->qstatOpts));

    ServerListQStat* favSL = new ServerListQStat(this);
    favSL_ = favSL;
    favSL->setOpts(&(opts_->servers));
    favSL->setQStatOpts(&(opts_->qstatOpts));

    allList_->setServerList(allSL_);
    connect(allSL_, SIGNAL(refreshStopped()), SLOT(refreshAllStopped()));
    allList_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    allList_->tree()->addAction(ui.actionConnect);
    allList_->tree()->addAction(ui.actionAddToFav);
    allList_->tree()->addAction(ui.actionRefreshAll);
    allList_->tree()->addAction(ui.actionRefreshSelected);
    
    favList_->setServerList(favSL_);
    favList_->tree()->setContextMenuPolicy(Qt::ActionsContextMenu);
    favList_->tree()->addAction(ui.actionConnect);
    favList_->tree()->addAction(ui.actionFavAdd);
    favList_->tree()->addAction(ui.actionFavEdit);
    favList_->tree()->addAction(ui.actionFavDelete);
    favList_->tree()->addAction(ui.actionRefreshSelected);
    favList_->tree()->addAction(ui.actionRefreshAll);

    updateActions();
}


MainWindow::~MainWindow()
{
    delete opts_;
}

void MainWindow::showOptions()
{
    OptionsDialog d;
    d.setOpts(opts_);
    if (d.exec() == QDialog::Rejected) return;
    saveOptions();
}

void MainWindow::quickConnect()
{
    launcher_.setServerID( ServerID( ui.qlServerEdit->text() ) );
    launcher_.setUserName( ui.qlPlayerEdit->text() );
    launcher_.setPassword( ui.qlPasswordEdit->text() );
    launcher_.launch();
}

void MainWindow::launchStatusChanged()
{
    cout << launcher_.executing() << endl;
    ui.quickConnectButton->setEnabled( !launcher_.executing() );
}

void MainWindow::favAdd()
{
    ServOptsDialog d;
    if (d.exec() == QDialog::Rejected) return;
    ServerOptionsList& list = opts_->servers;
    list[d.options().id] = d.options();
    saveOptions();
    syncFavList();
    favList_->forceUpdate();
    updateActions();
}

void MainWindow::favDelete()
{
    if (QMessageBox::question(this, tr("Delete a favorite"),
            tr("Continue to delete a favorite"), QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    ServerIDList sel = favList_->selection();
    ServerOptionsList& list = opts_->servers;
    for (ServerIDList::iterator it = sel.begin(); it != sel.end(); it++)
        list.erase(*it);
    saveOptions();
    syncFavList();
    favList_->forceUpdate();
    updateActions();
}

void MainWindow::syncFavList()
{
    ServerOptionsList& srclist = opts_->servers;
    ServerIDList& dstlist = favSL_->customServList();
    dstlist.clear();

    for (ServerOptionsList::iterator it = srclist.begin(); it != srclist.end(); it++)
        dstlist.push_back((*it).second.id);
    favSL_->update();
}

void MainWindow::saveOptions()
{
    // TODO save options to XML
}

void MainWindow::loadOptions()
{
    // TODO load options from XML

    opts_->qstatOpts.qstatPath = "/usr/bin/qstat";
    opts_->qstatOpts.masterServer = "master.urbanterror.net";
}

void MainWindow::refreshAll()
{
    ServListWidget* list = selectedListWidget();
    if (!list) return;
    ServerListCustom* sl = list->serverList();
    sl->refreshAll();
    ui.actionRefreshAll->setEnabled(false);
}

void MainWindow::refreshAllStopped()
{
    ui.actionRefreshAll->setEnabled(true);
}

void MainWindow::showAbout()
{
    AboutDialog d;
    d.exec();
}

void MainWindow::refreshSelected()
{
    ServerID* id = selected();
    if (!id) return;
    ServListWidget* list = selectedListWidget();
    if (!list) return;
    list->serverList()->refreshServer(*id);
}

ServerID* MainWindow::selected()
{
    ServListWidget* list = selectedListWidget();
    if (!list) return 0;
    ServerIDList sel = list->selection();
    if (sel.size() == 0) return 0;
    return &(sel.front());
}

void MainWindow::connectSelected()
{
    ServerID* id = selected();
    if (!id) return;

    ServerOptions& opts = opts_->servers[*id];

    launcher_.setServerID( *id );
    launcher_.setUserName("");
    launcher_.setPassword(opts.password);
    launcher_.setReferee(opts.refPassword);
    launcher_.setRcon(opts.rconPassword);
    launcher_.launch();

}

ServListWidget* MainWindow::selectedListWidget()
{
    QWidget* curw = ui.tabWidget->currentWidget();
    if (curw == ui.tabFav)
        return favList_;
    else if (curw == ui.tabAll)
        return allList_;
    else return 0;
}

void MainWindow::updateActions()
{
    QWidget* curw = selectedListWidget();
    ServerID* sel = selected();

    ui.actionAddToFav->setEnabled(curw == allList_ && sel);
    ui.actionConnect->setEnabled(sel);
    ui.actionFavAdd->setEnabled(curw == favList_);
    ui.actionFavDelete->setEnabled(curw == favList_ && sel);
    ui.actionFavEdit->setEnabled(curw == favList_ && sel);
    ui.actionRefreshSelected->setEnabled(sel);
}

void MainWindow::favEdit()
{
    ServerID* id = selected();
    if (!id) return;

    // FIXME i dont know why AV here, need to valgrind check
    ServerOptions opts = opts_->servers[*id];

    ServOptsDialog d(this, opts);
    if (d.exec() == QDialog::Rejected) return;

    opts_->servers[*id] = d.options();
    saveOptions();
    syncFavList();
    favList_->forceUpdate();

    updateActions();
}

void MainWindow::currentTabChanged(int)
{
    updateActions();
}