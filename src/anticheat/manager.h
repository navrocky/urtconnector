#pragma once

#include <QObject>

namespace anticheat
{

class anticheat;
class sshot_ftp_output;
class sshot_file_output;

class manager : public QObject
{
    Q_OBJECT
public:
    manager(QObject *parent = 0);

    void set_nick_name(const QString& val);

    void start();
    void stop();

private:
    void update_settings();

    anticheat* anticheat_;
    sshot_file_output* file_out_;
    sshot_ftp_output* ftp_out_;
};

}
