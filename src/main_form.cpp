#include <QMessageBox>

#include "main_form.h"
#include "ui_main_form.h"

main_form::main_form(QWidget* parent)
: QWidget(parent)
, ui_(new Ui_main_form)
{
    ui_->setupUi(this);
    connect(ui_->pushButton, SIGNAL(clicked()), SLOT(button_clicked()));
}

void main_form::button_clicked()
{
    QMessageBox::information(this, tr("Title"), tr("Hello world!"), QMessageBox::Ok, QMessageBox::Ok);
}


