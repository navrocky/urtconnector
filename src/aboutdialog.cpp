#include "ui_aboutdialog.h"
#include "aboutdialog.h"
AboutDialog::AboutDialog(QWidget *parent)
 : QDialog(parent),
   ui(new Ui::AboutDialogClass)
{
    ui->setupUi(this);
    setWindowTitle(tr("About UrTConnector"));

    //updateDialog();
}

AboutDialog::~AboutDialog() {}
