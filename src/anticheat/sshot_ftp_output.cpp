#include <QFtp>

#include <cl/syslog/syslog.h>

#include "sshot_ftp_output.h"

SYSLOG_MODULE("sshot_ftp_output")

namespace anticheat
{

sshot_ftp_output::sshot_ftp_output(QObject* parent)
: sshot_output(parent)
{
    ftp_ = new QFtp(this);
    connect(ftp_, SIGNAL(done(bool)), SLOT(ftp_done(bool)));
}

void sshot_ftp_output::set_connection_info(const server_id& addr, const QString& login,
    const QString& password)
{
    addr_ = addr;
    login_ = login;
    password_ = password;
}

void sshot_ftp_output::set_folder(const QString& folder)
{
    folder_ = folder;
}

bool sshot_ftp_output::can_send_now()
{
    return !ftp_->hasPendingCommands();
}

void sshot_ftp_output::start()
{
    connection_needed();
}

void sshot_ftp_output::stop()
{
    if (ftp_->state() != QFtp::Unconnected)
    {
        LOG_DEBUG << "Closing FTP connection";
        ftp_->close();
    }
}

void sshot_ftp_output::connection_needed()
{
    if (ftp_->state() != QFtp::Unconnected)
        return;

    LOG_DEBUG << "Connecting to FTP server \"%1\"", addr_.address().toStdString();
    ftp_->connectToHost(addr_.ip_or_host(), addr_.port());
    ftp_->login(login_, password_);
    ftp_->mkdir(folder_);
    ftp_->cd(folder_);
}

void sshot_ftp_output::send_file(const QString& name, const QByteArray& data)
{
    connection_needed();
    ftp_->put(data, name);
}

void sshot_ftp_output::ftp_done(bool error)
{
    if (error)
        LOG_ERR << "Ftp error %1", ftp_->errorString().toStdString();
    else
        LOG_DEBUG << "Ftp command done.";
}

}