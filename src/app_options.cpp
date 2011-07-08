
#include <QDate>
#include <QLocale>
#include <QSettings>
#include <QTranslator>
#include <QLibraryInfo>

#include "config.h"
#include "app_options.h"
#include "pointers.h"

namespace
{
const QString c_style_sheet_file = "style_sheet_file";
}


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
    return part()->value("binary_path", "urbanterror").toString();
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
    part()->setValue("geoip_database", file);
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

int app_settings::number_in_history() const
{
    return part()->value("number_in_history", 900).toInt();
}

void app_settings::set_number_in_history(int lines)
{
    part()->setValue("number_in_history", lines);
}

int app_settings::days_in_history() const
{
    return part()->value("days_in_history", 90).toInt();
}

void app_settings::set_days_in_history(int days)
{
    part()->setValue("days_in_history", days);
}

bool app_settings::history_grouping() const
{
    return part()->value("history_grouping", true).toBool();
}

void app_settings::set_history_grouping(bool val)
{
    part()->setValue("history_grouping", val);
}

bool app_settings::update_before_connect() const
{
    return part()->value("update_before_connect", true).toBool();
}

void app_settings::set_update_before_connect(bool b)
{
    part()->setValue("update_before_connect", b);
}

bool app_settings::christmas_mode() const
{
    int month = QDate::currentDate().month();
    return ( month == 1 || month == 12 ) && use_holiday_mode();
}

bool app_settings::use_holiday_mode() const
{
    return part()->value("holiday", true).toBool();
}

void app_settings::set_holiday_mode(bool b)
{
    part()->setValue("holiday", b);
}

bool app_settings::clear_offline() const
{
    return part()->value("clear_offline", true).toBool();
}

void app_settings::set_clear_offline(bool b)
{
    part()->setValue("clear_offline", b);
}

QString app_settings::country_name() const
{
    return part()->value( "country_name", QLocale::system().name() ).toString();
}

void app_settings::set_country_name(const QString& country)
{
    part()->setValue("country_name", country);
}

bool app_settings::use_mumble_overlay() const
{
    return part()->value("use_mumble_overlay", false).toBool();
}

void app_settings::set_use_mumble_overlay(bool val)
{
    part()->setValue("use_mumble_overlay", val);
}

QString app_settings::mumble_overlay_bin() const
{
    return part()->value("mumble_overlay_bin", "/usr/bin/mumble-overlay").toString();
}

void app_settings::set_mumble_overlay_bin(const QString& val)
{
    part()->setValue("mumble_overlay_bin", val);
}

QString app_settings::style_sheet_file() const
{
    return part()->value(c_style_sheet_file, "").toString();
}

void app_settings::reset_style_sheet_file()
{
    part()->remove(c_style_sheet_file);
}

void app_settings::set_style_sheet_file(const QString& val)
{
    part()->setValue(c_style_sheet_file, val);
}


////////////////////////////////////////////////////////////////////////////////
// clip_settings

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



boost::shared_ptr<QTranslator> system_translator(const QString& code)
{
    boost::shared_ptr<QTranslator> ret( new QTranslator );
    ret->load( "qt_" + code, QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    return ret;
}

boost::shared_ptr< QTranslator > local_translator(const QString& code)
{
    boost::shared_ptr<QTranslator> ret( new QTranslator );
  
#if defined(Q_OS_UNIX)
    ret->load( "urtconnector_" + code, "/usr/share/urtconnector/translations");
#elif defined(Q_OS_WIN)
    ret->load(code);
#elif defined(Q_OS_MAC)
    // FIXME i don't know how do this on mac
    ret->load(code);
#endif
    return ret;
}

