#include <iostream>

#include <QAction>
#include <QMenu>
#include <QTreeView>
#include <QBoxLayout>
#include <QTreeWidget>
#include <QMessageBox>

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

    favList_ = new ServListWidget(ui.tabFav);
    dynamic_cast<QBoxLayout*>(ui.tabFav->layout())->insertWidget(0, favList_);

    connect(ui.actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );
    connect(ui.actionQuickConnect, SIGNAL( triggered() ), SLOT( quickConnect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launchStatusChanged()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launchStatusChanged()));
    connect(ui.actionFavAdd, SIGNAL(triggered()), SLOT(favAdd()));
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
    allSL->setOpts(&(opts_->servers()));
    allSL->setQStatOpts(&(opts_->qstatOpts));

    ServerListQStat* favSL = new ServerListQStat(this);
    favSL_ = favSL;
    favSL->setOpts(&(opts_->servers()));
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
    favList_->tree()->addAction(ui.actionFavDelete);
    favList_->tree()->addAction(ui.actionRefreshSelected);
    favList_->tree()->addAction(ui.actionRefreshAll);

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
    ServerOptionsList& list = opts_->servers();
    list[d.options().id] = d.options();
    saveOptions();
    syncFavList();
    favList_->forceUpdate();
}

void MainWindow::favDelete()
{
    if (QMessageBox::question(this, tr("Delete a favorite"),
            tr("Continue to delete a favorite"), QMessageBox::Ok | QMessageBox::Cancel) != QMessageBox::Ok)
        return;
    ServerIDList sel = favList_->selection();
    ServerOptionsList& list = opts_->servers();
    for (ServerIDList::iterator it = sel.begin(); it != sel.end(); it++)
        list.erase(*it);
    saveOptions();
    syncFavList();
    favList_->forceUpdate();
}

void MainWindow::syncFavList()
{
    ServerOptionsList& srclist = opts_->servers();
    ServerIDList& dstlist = favSL_->customServList();
    dstlist.clear();

    for (ServerOptionsList::iterator it = srclist.begin(); it != srclist.end(); it++)
    {
        dstlist.push_back((*it).second.id);
    }
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
    ServListWidget* list = dynamic_cast<ServListWidget*>(ui.tabWidget->currentWidget());
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
    ServListWidget* list = dynamic_cast<ServListWidget*>(ui.tabWidget->currentWidget());
    if (!list) return;
    list->serverList()->refreshServer(*id);
}

ServerID* MainWindow::selected()
{
    ServListWidget* list = dynamic_cast<ServListWidget*>(ui.tabWidget->currentWidget());
    if (!list) return 0;
    ServerIDList sel = list->selection();
    if (sel.size() == 0) return 0;
    return &(sel.front());
}

void MainWindow::connectSelected()
{
    ServerID* id = selected();
    if (!id) return;
    favSL_->refreshServer(*id);

    launcher_.setServerID( ServerID( ui.qlServerEdit->text() ) );
    launcher_.setUserName( ui.qlPlayerEdit->text() );
    launcher_.setPassword( ui.qlPasswordEdit->text() );
    launcher_.launch();

}

ServListWidget* MainWindow::selectedListWidget()
{
    
}
