#pragma once

#include <QObject>

#include <common/server_id.h>

class QEvent;
class QFtp;

/*! Anticheat sends a screenshots with md5 to the public ftp server */

class anticheat : public QObject
{
    Q_OBJECT
public:
    anticheat(QObject* parent = NULL);
    
    // interval between screenshots in milliseconds
    int interval() const {return interval_;}
    void set_interval(int val);

    int quality() const {return quality_;}
    void set_quality(int val);

    const QString& nick_name() const {return nick_name_;}
    void set_nick_name(const QString& val);

    void set_ftp_connection_info(const server_id& addr, const QString& login,
        const QString& password);
    void set_ftp_folder(const QString& folder);

    void start();
    void stop();

    virtual bool event(QEvent* e);

private slots:
    void ftp_done(bool error);

private:
    void update_timer();
    void screen_shot();
    void ftp_connection_needed();

    int interval_;
    int quality_;
    int timer_;
    QFtp* ftp_;
    QString nick_name_;
    server_id addr_;
    QString login_;
    QString password_;
    QString ftp_folder_;
};