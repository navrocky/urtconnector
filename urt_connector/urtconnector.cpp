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
    QString command;
    command = this->ui->ioexeEdit->text();
    if (this->ui->passwordCheckBox->isChecked())
    {
        command += " +password \"" + this->ui->passwordEdit->text() + "\"";
    }
    command += " +connect " + this->ui->serverEdit->text();
    command += " +name \"" + this->ui->playernameEdit->text() + "\"";

    system(command.toAscii());
}

void urtconnector::getUrTExe()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Urban Terror executable"), "", tr("i386 (*.i386);;.exe (*.exe);;x86_64 (*x86_64);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        this->ui->ioexeEdit->setText(fileName);
    }
}
