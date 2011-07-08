#include <QMessageBox>

#include "main_form.h"
#include "ui_main_form.h"

main_form::main_form(QWidget* parent)
: QWidget(parent)
, ui_(new Ui_main_form)
{
    ui_->setupUi(this);
    connect(ui_->button, SIGNAL(clicked()), SLOT(button_clicked()));
}

void main_form::button_clicked()
{
//    QMessageBox::information(this, ui_->edit1->text(), ui_->edit2->text(), QMessageBox::Ok, QMessageBox::Ok);
    ui_->edit2->setText(ui_->edit1->text());
}


