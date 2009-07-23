#include "utconnector.h"
#include "ui_utconnector.h"
#include <QFileDialog>


utconnector::utconnector(QWidget *parent)
    : QWidget(parent), ui(new Ui::utconnectorClass)
{
    ui->setupUi(this);
    connect(ui->connectButton, SIGNAL(clicked()), this, SLOT(launchUrbanTerror()));
    connect(ui->ioexeButton, SIGNAL(clicked()), this, SLOT(getUrTExe()));
}

utconnector::~utconnector()
{
    delete ui;
}

void utconnector::launchUrbanTerror()
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

void utconnector::getUrTExe()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Urban Terror executable"), "", tr("i386 (*.i386);;.exe (*.exe);;x86_64 (*x86_64);;All Files (*)"));
    if (!fileName.isEmpty())
    {
        this->ui->ioexeEdit->setText(fileName);
    }
}
