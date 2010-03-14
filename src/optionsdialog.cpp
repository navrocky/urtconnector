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
    connect( ui.qstat_binary_choose_button, SIGNAL( clicked() ), SLOT( choose_qstat_binary() ));
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
    ui.qstat_binary_edit->setText( opts_->qstat_opts.qstat_path );
}

void options_dialog::accept()
{
    QDialog::accept();
    opts_->binary_path = ui.binaryEdit->text();
    opts_->adv_cmd_line = ui.advCmdEdit->text();
    opts_->use_adv_cmd_line = ui.advCmdBox->isChecked();
    opts_->qstat_opts.qstat_path = ui.qstat_binary_edit->text();
}

void options_dialog::choose_binary()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Urban Terror executable"), "",
      tr("Executables (*.i386 *.x86_64 *.exe);;All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.binaryEdit->setText(fileName);
}

void options_dialog::choose_qstat_binary()
{
    QString file_name = QFileDialog::getOpenFileName(this,
      tr("QStat binary"), "",
      tr("Executables (*);;All Files (*)"));
    if (file_name.isEmpty()) return;

    ui.qstat_binary_edit->setText(file_name);
}

void options_dialog::insert_file_path()
{
    QString fileName = QFileDialog::getOpenFileName(this,
      tr("Select file to insert"), "",
      tr("All Files (*)"));
    if (fileName.isEmpty()) return;

    ui.advCmdEdit->insert(fileName);
}
