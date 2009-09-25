#include <iostream>

#include <QAction>
#include <QMenu>
#include <QTreeView>
#include <QBoxLayout>

#include "mainwindow.h"
#include "optionsdialog.h"
#include "launcher.h"
#include "exception.h"
#include "servoptsdialog.h"
#include "pushbuttonactionlink.h"
#include "serverlistqstat.h"
#include "aboutdialog.h"
#include "qstatoptions.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   opts_( new AppOptions() ),
   launcher_(opts_)
{
    ui.setupUi(this);

    allList_ = new ServListWidget(ui.tabAll);
    dynamic_cast<QBoxLayout*>(ui.tabAll->layout())->insertWidget(0, allList_);

    favList_ = new ServListWidget(ui.tabFav);
    dynamic_cast<QBoxLayout*>(ui.tabFav->layout())->insertWidget(0, favList_);

    connect(ui.actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );
    connect(ui.actionQuickConnect, SIGNAL( triggered() ), SLOT( quickConnect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launchStatusChanged()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launchStatusChanged()));
    connect(ui.actionFavAdd, SIGNAL(triggered()), SLOT(favAdd()));
    connect(ui.actionFavDelete, SIGNAL(triggered()), SLOT(favDelete()));
    connect(ui.actionRefreshAll, SIGNAL(triggered()), SLOT(refreshAll()));
    connect(ui.actionAbout, SIGNAL(triggered()), SLOT(showAbout()));

    new PushButtonActionLink(ui.favAddButton, ui.actionFavAdd);
    new PushButtonActionLink(ui.favDeleteButton, ui.actionFavDelete);
    new PushButtonActionLink(ui.quickConnectButton, ui.actionQuickConnect);
    new PushButtonActionLink(ui.favConnectButton, ui.actionConnectToFavorite);
    new PushButtonActionLink(ui.refreshAllButton, ui.actionRefreshAll);

    loadOptions();

    allSL_ = new ServerListQStat(this);
    favSL_ = new ServerListQStat(this);

    dynamic_cast<ServerListQStat*>(allSL_)->setQStatOpts(&(opts_->qstatOpts));

    allList_->setServerList(allSL_);
    connect(allSL_, SIGNAL(refreshStopped()), SLOT(refreshAllStopped()));
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
    list[d.options().uid] = d.options();
    saveOptions();

    syncFavList();
}

void MainWindow::favDelete()
{
}

void MainWindow::syncFavList()
{
}

void MainWindow::saveOptions()
{
    // TODO save options to XML
}

void MainWindow::loadOptions()
{
    // TODO load options from XML

    opts_->useAdvCmdLine = true;
    opts_->advCmdLine = "";
    opts_->qstatOpts.masterServer = "master.urbanterror.net";
    opts_->qstatOpts.qstatPath = "/usr/bin/qstat";

}

void MainWindow::refreshAll()
{
    allSL_->refreshAll();
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


