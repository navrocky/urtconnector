#include <QLineEdit>
#include <QGroupBox>
#include "ui_optionsdialog.h"
#include "optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent)
 : QDialog(parent),
   ui(new Ui::OptionsDialogClass)
{
    ui->setupUi(this);
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
    opts_->setBinaryPath( ui->binaryEdit->text() );
    opts_->setAdvCmdLine( ui->advCmdEdit->text() );
    opts_->setUseAdvCmdLine( ui->advCmdBox->isChecked() );
}


