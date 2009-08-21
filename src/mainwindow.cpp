#include <iostream>

#include <QAction>
#include <QMenu>
#include <QTreeView>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include "launcher.h"
#include "exception.h"
#include "servoptsdialog.h"
#include "pushbuttonactionlink.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   ui(new Ui::MainWindowClass),
   opts_( new AppOptions() ),
   launcher_(opts_)
{
    ui->setupUi(this);

    connect(ui->actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );
    connect(ui->actionQuickConnect, SIGNAL( triggered() ), SLOT( quickConnect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launchStatusChanged()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launchStatusChanged()));
    connect(ui->actionFavAdd, SIGNAL(triggered()), SLOT(favAdd()));
    connect(ui->actionFavDelete, SIGNAL(triggered()), SLOT(favDelete()));

    new PushButtonActionLink(ui->favAddButton, ui->actionFavAdd);
    new PushButtonActionLink(ui->favDeleteButton, ui->actionFavDelete);
    new PushButtonActionLink(ui->quickConnectButton, ui->actionQuickConnect);
    new PushButtonActionLink(ui->favConnectButton, ui->actionConnectToFavorite);

    loadOptions();
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
    launcher_.setServerID( ServerID( ui->qlServerEdit->text() ) );
    launcher_.setUserName( ui->qlPlayerEdit->text() );
    launcher_.setPassword( ui->qlPasswordEdit->text() );
    launcher_.launch();
}

void MainWindow::launchStatusChanged()
{
    cout << launcher_.executing() << endl;
    ui->quickConnectButton->setEnabled( !launcher_.executing() );
}

void MainWindow::favAdd()
{
    ServOptsDialog d;
    if (d.exec() == QDialog::Rejected) return;
    ServerOptionsList& list = opts_->servers();
    list[d.options().uid()] = d.options();
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
}