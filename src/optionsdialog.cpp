#include <QLineEdit>
#include <QGroupBox>
#include <QFileDialog>

#include "optionsdialog.h"

options_dialog::options_dialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);

    connect( ui.selectBinButton, SIGNAL( clicked() ), SLOT( choose_binary() ));
    connect( ui.insertFileButton, SIGNAL( clicked() ), SLOT( insert_file_path() ));
}

options_dialog::~options_dialog()
{
}

void options_dialog::set_opts(app_options_ptr value)
{
    opts_ = value;
    update_dialog();
}

void options_dialog::update_dialog()
{
    ui.binaryEdit->setText( opts_->binary_path );
    ui.advCmdEdit->setText( opts_->adv_cmd_line );
    ui.advCmdBox->setChecked( opts_->use_adv_cmd_line );
}

void options_dialog::accept()
{
    QDialog::accept();
    opts_->binary_path = ui.binaryEdit->text();
    opts_->adv_cmd_line = ui.advCmdEdit->text();
    opts_->use_adv_cmd_line = ui.advCmdBox->isChecked();
}

void options_dialog::choose_binary()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Urban Terror executable"), "",
      tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.binaryEdit->setText(fileName);
}

void options_dialog::insert_file_path()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Select file to insert"), "",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.advCmdEdit->insert(fileName);
}
