#pragma once

#include <QString>

#include <settings/settings.h>
#include <common/server_id.h>

namespace anticheat
{

class settings : public settings_uid_provider<settings>
{
public:

    /// time interval between screenshots in seconds
    int interval() const;
    void set_interval(int val);

    /// screenshots jpeg quality
    int quality() const;
    void set_quality(int val);

    bool use_ftp() const;
    void set_use_ftp(bool val);

    /// ftp connection address
    server_id ftp_address() const;
    void set_ftp_address(const server_id& val);

    /// ftp login name
    QString ftp_login() const;
    void set_ftp_login(const QString& val);

    /// ftp login password
    QString ftp_password() const;
    void set_ftp_password(const QString& val);

    /// ftp folder
    QString ftp_folder() const;
    void set_ftp_folder(const QString& val);

    /// send only md5
    bool send_only_md5() const;
    void set_send_only_md5(bool val);

    /// Use local folder for screen shots store
    bool use_local_folder() const;
    void set_use_local_folder(bool val);

    /// Local folder for screen shots
    QString local_folder() const;
    void set_local_folder(const QString& val);

};

}


