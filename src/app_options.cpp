
#include <QSettings>

#include "config.h"
#include "app_options.h"


bool app_settings::start_hidden() const
{
    return part()->value("start_hidden", false).toBool();
}

void app_settings::set_start_hidden(bool b)
{
    part()->setValue("start_hidden", b);
}


bool app_settings::use_adv_cmd_line() const
{
    return part()->value("use_adv_cmd_line", false).toBool();
}

void app_settings::set_use_adv_cmd_line(bool b)
{
    part()->setValue("use_adv_cmd_line", b);
}


QString app_settings::adv_cmd_line() const
{
    return part()->value("adv_cmd_line", QString() ).toString();
}

void app_settings::set_adv_cmd_line(const QString& line)
{
    part()->setValue("adv_cmd_line", line);
}


bool app_settings::separate_x() const
{
    return part()->value("separate_xsession", true).toBool();
}

void app_settings::set_separate_x(bool b)
{
    part()->setValue("separate_xsession", b);
}


QString app_settings::binary_path() const
{
    return part()->value("binary_path", QString()).toString();
}

void app_settings::set_binary_path(const QString& path)
{
    part()->setValue("binary_path", path);
}


QString app_settings::geoip_database() const
{
    return part()->value("geoip_database", QString(URT_DATADIR) + "GeoIP.dat").toString();
}

void app_settings::set_geoip_database(const QString& file)
{
    part()->value("center_current_row", true).toBool();
}


bool app_settings::center_current_row() const
{
    return part()->value("center_current_row", true).toBool();
}

void app_settings::set_center_current_row(bool b)
{
    part()->setValue("center_current_row", b);
}


bool app_settings::keep_history() const
{
    return part()->value("keep_history", true).toBool();
}

bool app_settings::set_keep_history(bool b)
{
    part()->setValue("keep_history", b);
}


unsigned int app_settings::number_in_history() const
{
    return part()->value("number_in_history", 30).toUInt();
}

void app_settings::set_number_in_history(unsigned int lines)
{
    part()->setValue("number_in_history", lines);
}


bool app_settings::update_before_connect() const
{
    return part()->value("update_before_connect", true).toBool();
}

void app_settings::set_update_before_connect(bool b)
{
    part()->setValue("update_before_connect", b);
}




bool clip_settings::watching() const
{
    return part()->value("looking_for", true).toBool();
}

void clip_settings::set_watching(bool b)
{
    part()->setValue("looking_for", b);
}


QString clip_settings::regexp() const
{
    return part()->value("regexp",
        "(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})"
        "(:(\\d{1,5}))?(.+pass[^\\s]*\\s+([^\\s]+))?").toString();
}

void clip_settings::set_regexp(const QString& rx)
{
    part()->setValue("regexp", rx);
}

void clip_settings::reset_regexp()
{
    part()->remove("regexp");
}

int clip_settings::host() const
{
    return part()->value("host", 1).toInt();
}

void clip_settings::set_host(int host)
{
    part()->setValue("host", host);
}


int clip_settings::port() const
{
    return part()->value("port", 3).toInt();
}

void clip_settings::set_port(int port)
{
    part()->setValue("port", port);
}


int clip_settings::password() const
{
    return part()->value("password", 5).toInt();
}


void clip_settings::set_password(int pass)
{
    part()->setValue("password", pass);
}





