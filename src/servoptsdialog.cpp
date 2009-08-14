#include "ui_servoptsdialog.h"

#include "servoptsdialog.h"

ServOptsDialog::ServOptsDialog(QWidget *parent)
 : QDialog(parent),
   ui(new Ui::ServOptsDialogClass)
{
    ui->setupUi(this);
}


ServOptsDialog::~ServOptsDialog()
{
}


