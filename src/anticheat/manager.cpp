#include "anticheat.h"
#include "settings.h"
#include "sshot_file_output.h"
#include "sshot_ftp_output.h"

#include "manager.h"

namespace anticheat
{

manager::manager(QObject *parent)
: QObject(parent)
{
    ftp_out_ = new sshot_ftp_output(this);
    file_out_ = new sshot_file_output(this);

    anticheat_ = new anticheat(this);
    anticheat_->add_output(file_out_);
    anticheat_->add_output(ftp_out_);
}

void manager::set_nick_name(const QString& val)
{
    anticheat_->set_nick_name(val);
}

void manager::start()
{
    update_settings();
    anticheat_->start();
}

void manager::stop()
{
    anticheat_->stop();
}

void manager::update_settings()
{
    settings s;

    anticheat_->set_interval(s.interval());
    anticheat_->set_quality(s.quality());

    ftp_out_->set_enabled(s.use_ftp());
    ftp_out_->set_connection_info(s.ftp_address(), s.ftp_login(), s.ftp_password());
    ftp_out_->set_folder(s.ftp_folder());

    file_out_->set_enabled(s.use_local_folder());
    file_out_->set_folder(s.local_folder());
}

}
