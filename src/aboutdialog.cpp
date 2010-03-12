#include "ui_aboutdialog.h"
#include "aboutdialog.h"
about_dialog::about_dialog(QWidget *parent)
 : QDialog(parent),
   ui_(new Ui::AboutDialogClass)
{
    ui_->setupUi(this);
    setWindowTitle(tr("About UrTConnector"));

    //updateDialog();
}

about_dialog::~about_dialog() {}
