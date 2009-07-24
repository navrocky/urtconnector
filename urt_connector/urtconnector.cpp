#include "urtconnector.h"
#include "ui_urtconnector.h"
#include <QFileDialog>


urtconnector::urtconnector(QWidget *parent)
    : QWidget(parent), ui(new Ui::urtconnectorClass)
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(launchUrbanTerror()));
    connect(ui->ioexeButton, SIGNAL(clicked()), this, SLOT(getUrTExe()));
}

urtconnector::~urtconnector()
{
    delete ui;
}

void urtconnector::launchUrbanTerror()
{
    QStringList arguments;
    arguments << "+password" << "\"" + this->ui->passwordEdit->text() + "\"";
    arguments << "+connect" << "\"" + this->ui->serverEdit->text() + "\"";
    arguments << "+name" << "\"" + this->ui->playernameEdit->text() + "\"";

    process.start(this->ui->ioexeEdit->text(), arguments);
}

void urtconnector::getUrTExe()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Urban Terror executable"), "", tr("i386 (*.i386);;.exe (*.exe);;x86_64 (*x86_64);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        this->ui->ioexeEdit->setText(fileName);
    }
}
