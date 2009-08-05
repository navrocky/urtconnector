#include <QAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include "launcher.h"
#include "exception.h"

#include <iostream>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   ui(new Ui::MainWindowClass),
   opts_( new AppOptions() ),
   launcher_(opts_)
{
    ui->setupUi(this);


    connect(ui->actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );
    connect(ui->quickConnectButton, SIGNAL( clicked() ), SLOT( quickConnect() ) );
    connect(&launcher_, SIGNAL(started()), SLOT(launchStatusChanged()));
    connect(&launcher_, SIGNAL(finished()), SLOT(launchStatusChanged()));
}


MainWindow::~MainWindow()
{
    delete opts_;
}

void MainWindow::showOptions()
{
    OptionsDialog d;
    d.setOpts(opts_);
    d.exec();
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


