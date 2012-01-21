#pragma once

#include <QString>

#include <settings/settings_generator.h>
#include <common/server_id.h>

namespace anticheat
{
    
#define ANTICHEAT_SETTINGS \
    ((interval, int, 30)) \
    ((quality, int, 50)) \
    ((use_ftp, bool, false)) \
    ((ftp_address, server_id, server_id())) \
    ((ftp_login, QString, QString())) \
    ((ftp_password, QString, QString())) \
    ((ftp_folder, QString, QString("screenshots"))) \
    ((send_only_md5, bool, true)) \
    ((use_local_folder, bool, false)) \
    ((local_folder, QString, QString("screenshots")))
    
SETTINGS_GENERATE_CLASS(settings, ANTICHEAT_SETTINGS)

}


