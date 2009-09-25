#include "servoptsdialog.h"
#include "exception.h"

ServOptsDialog::ServOptsDialog(QWidget *parent)
 : QDialog(parent)
{
    ui.setupUi(this);
    setWindowTitle(tr("New server favorite"));

    opts_.uid = QUuid::createUuid();

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
    ServerID id;
    id.setHostName(ui.hostnameEdit->text());
    id.setIp(ui.ipEdit->text());
    id.setPort(ui.portEdit->text());
    if (id.isEmpty())
        throw Exception(tr("Server address must be non empty"));

    opts_.id = id;
    opts_.name = ui.nameEdit->text();
    opts_.rconPassword = ui.rconEdit->text();
    opts_.refPassword = ui.refEdit->text();
    opts_.comment = ui.commentEdit->toPlainText();
    QDialog::accept();
}

void ServOptsDialog::updateDialog()
{
    ui.hostnameEdit->setText(opts_.id.hostName());
    ui.ipEdit->setText(opts_.id.ip());
    int port = opts_.id.port();
    if (port != 0)
        ui.portEdit->setText(QString("%1").arg(port));
    else
        ui.portEdit->setText(QString::null);
    ui.nameEdit->setText(opts_.name);
    ui.rconEdit->setText(opts_.rconPassword);
    ui.refEdit->setText(opts_.refPassword);
    ui.commentEdit->setPlainText(opts_.comment);
}




