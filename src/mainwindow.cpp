#include <QAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"
#include "launcher.h"
#include "exception.h"

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   ui(new Ui::MainWindowClass),
   opts_( new AppOptions() )
{
    ui->setupUi(this);



    connect(ui->actionOptions, SIGNAL( triggered() ), SLOT( showOptions() ) );

    connect(ui->quickConnectButton, SIGNAL( clicked() ), SLOT( quickConnect() ) );




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
    Launcher l(opts_);
    l.setServerID( ServerID( ui->qlServerEdit->text() ) );
    l.setUserName( ui->qlPlayerEdit->text() );
    l.setPassword( ui->qlPasswordEdit->text() );
    l.launch();
}


