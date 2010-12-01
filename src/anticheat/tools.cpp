#include "sshot_ftp_output.h"
#include "sshot_file_output.h"
#include "settings.h"

#include "tools.h"

namespace anticheat
{

anticheat* create_anticheat(const QString& player_name, QObject* parent)
{
    settings s;
    anticheat* res = new anticheat(parent);

    res->set_nick_name(player_name);
    res->set_quality(s.quality());
    res->set_interval(s.interval() * 1000);

    if (s.use_ftp())
    {
        sshot_ftp_output* ftp = new sshot_ftp_output(res);
        ftp->set_connection_info(s.ftp_address(), s.ftp_login(), s.ftp_password());
        ftp->set_folder(s.ftp_folder());
        res->add_output(ftp);
    }

    if (s.use_local_folder())
    {
        sshot_file_output* file = new sshot_file_output(res);
        file->set_folder(s.local_folder());
        res->add_output(file);
    }

    return res;
}

}
