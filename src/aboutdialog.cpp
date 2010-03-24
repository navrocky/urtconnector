#include "config.h"

#include "ui_aboutdialog.h"
#include "aboutdialog.h"

about_dialog::about_dialog(QWidget *parent)
: QDialog(parent)
, ui_(new Ui::AboutDialogClass)
{
    ui_->setupUi(this);
    ui_->label_version->setText(tr("version %1 \"%2\"").arg(URT_VERSION).arg(URT_CODENAME));
}

about_dialog::~about_dialog()
{
}
