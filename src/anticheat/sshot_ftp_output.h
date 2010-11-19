#pragma once

#include <QString>
#include <common/server_id.h>

#include "sshot_output.h"

class QFtp;


namespace anticheat
{

class sshot_ftp_output : public sshot_output
{
    Q_OBJECT
public:
    sshot_ftp_output(QObject* parent = NULL);

    virtual bool can_send_now();
    virtual void send_file(const QString& name, const QByteArray& data);
    virtual void start();
    virtual void stop();

    void set_connection_info(const server_id& addr, const QString& login,
        const QString& password);
    void set_folder(const QString& folder);

private slots:
    void ftp_done(bool);

private:
    void connection_needed();

    QFtp* ftp_;
    server_id addr_;
    QString login_;
    QString password_;
    QString folder_;
};

}