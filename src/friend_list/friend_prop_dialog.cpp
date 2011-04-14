#include "friend_prop_dialog.h"

#include <QTextEdit>
#include <common/exception.h>

#include "ui_friend_prop_dialog.h"

friend_prop_dialog::friend_prop_dialog(QWidget* parent)
: QDialog(parent)
, ui_(new Ui_friend_prop_dialog)
{
    ui_->setupUi(this);

#if (QT_VERSION >= QT_VERSION_CHECK(4, 7, 0))
    ui_->expr_edit->setPlaceholderText(tr("Not used if empty"));
#endif
}

void friend_prop_dialog::set_rec(const friend_record& rec)
{
    rec_ = rec;
    update_contents();
}

void friend_prop_dialog::update_contents()
{
    ui_->name_edit->setText(rec_.nick_name());
    ui_->expr_edit->setText(rec_.expression());
    ui_->comment_edit->setText(rec_.comment());
}

void friend_prop_dialog::accept()
{
    if (ui_->name_edit->text().trimmed().isEmpty())
        throw qexception(tr("Nick name field must be defined"));
    
    rec_.set_nick_name(ui_->name_edit->text().trimmed());
    rec_.set_expression(ui_->expr_edit->text().trimmed());
    rec_.set_comment(ui_->comment_edit->toPlainText().trimmed());
    QDialog::accept();
}
