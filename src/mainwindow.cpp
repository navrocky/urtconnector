#include <QAction>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "optionsdialog.h"

MainWindow::MainWindow(QWidget *parent)
 : QMainWindow(parent),
   ui(new Ui::MainWindowClass),
   opts_( new AppOptions() )
{
    ui->setupUi(this);

    

    connect(ui->actionOptions, SIGNAL( triggered(bool) ), SLOT( showOptions(bool) ) );
}


MainWindow::~MainWindow()
{
}

void MainWindow::showOptions(bool)
{
    OptionsDialog d;
    d.setOpts(opts_);
    d.exec();
}


