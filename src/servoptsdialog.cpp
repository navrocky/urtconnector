#include "servoptsdialog.h"
#include "exception.h"

ServOptsDialog::ServOptsDialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("New server favorite"));
    updateDialog();
}

ServOptsDialog::ServOptsDialog(QWidget * parent, const ServerOptions & src)
  : QDialog(parent),
    opts_(src)
{
    ui.setupUi(this);
    setWindowTitle(tr("Server favorite options"));
    updateDialog();
}

ServOptsDialog::~ServOptsDialog()
{
}

void ServOptsDialog::accept()
{
    server_id id(ui.addressEdit->text());
    if (id.isEmpty())
        throw qexception(tr("Server address must be non empty"));

    opts_.id = id;
    opts_.name = ui.nameEdit->text();
    opts_.password = ui.passwordEdit->text();
    opts_.rconPassword = ui.rconEdit->text();
    opts_.refPassword = ui.refEdit->text();
    opts_.comment = ui.commentEdit->toPlainText();
    QDialog::accept();
}

void ServOptsDialog::updateDialog()
{
    if (!(opts_.id.isEmpty()))
        ui.addressEdit->setText(opts_.id.address());
    else
        ui.addressEdit->setText(QString());

    ui.nameEdit->setText(opts_.name);
    ui.passwordEdit->setText(opts_.password);
    ui.rconEdit->setText(opts_.rconPassword);
    ui.refEdit->setText(opts_.refPassword);
    ui.commentEdit->setPlainText(opts_.comment);
}




