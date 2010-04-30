#include "server_options_dialog.h"
#include "exception.h"

server_options_dialog::server_options_dialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("New server favorite"));
    update_dialog();
}

server_options_dialog::server_options_dialog(QWidget * parent, const server_options & src)
  : QDialog(parent),
    opts_(src)
{
    ui.setupUi(this);
    setWindowTitle(tr("Server favorite options"));
    update_dialog();
}

void server_options_dialog::accept()
{
    server_id id(ui.addressEdit->text());
    if (id.is_empty())
        throw qexception(tr("Server address must be non empty"));

    opts_.id = id;
    opts_.name = ui.nameEdit->text();
    opts_.password = ui.passwordEdit->text();
    opts_.rcon_password = ui.rconEdit->text();
    opts_.ref_password = ui.refEdit->text();
    opts_.comment = ui.commentEdit->toPlainText();
    QDialog::accept();
}

void server_options_dialog::update_dialog()
{
    if (!(opts_.id.is_empty()))
        ui.addressEdit->setText(opts_.id.address());
    else
        ui.addressEdit->setText(QString());

    ui.nameEdit->setText(opts_.name);
    ui.passwordEdit->setText(opts_.password);
    ui.rconEdit->setText(opts_.rcon_password);
    ui.refEdit->setText(opts_.ref_password);
    ui.commentEdit->setPlainText(opts_.comment);
}




