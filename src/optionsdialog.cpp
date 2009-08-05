#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>

#include "ui_optionsdialog.h"
#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent)
 : QDialog(parent),
   ui(new Ui::OptionsDialogClass)
{
    ui->setupUi(this);

    connect( ui->selectBinButton, SIGNAL( clicked() ), SLOT( chooseBinary() ));
    connect( ui->insertFileButton, SIGNAL( clicked() ), SLOT( insertFilePath() ));
}


OptionsDialog::~OptionsDialog()
{
}

void OptionsDialog::setOpts(AppOptionsPtr value)
{
    opts_ = value;
    updateDialog();
}

void OptionsDialog::updateDialog()
{
    ui->binaryEdit->setText( opts_->binaryPath() );
    ui->advCmdEdit->setText( opts_->advCmdLine() );
    ui->advCmdBox->setChecked( opts_->useAdvCmdLine() );
}

void OptionsDialog::accept()
{
    QDialog::accept();
    opts_->setBinaryPath( ui->binaryEdit->text() );
    opts_->setAdvCmdLine( ui->advCmdEdit->text() );
    opts_->setUseAdvCmdLine( ui->advCmdBox->isChecked() );
}

void OptionsDialog::chooseBinary()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Urban Terror executable"), "",
      tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fileName.isEmpty()) return;

    ui->binaryEdit->setText(fileName);
}

void OptionsDialog::insertFilePath()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Select file to insert"), "",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    ui->advCmdEdit->insert(fileName);
}


