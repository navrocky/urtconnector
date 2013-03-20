#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QDateTime>
#include <QString>
#include <QtGlobal>

#include <settings/settings_generator.h>


class QTranslator;

#ifdef Q_OS_WIN32
#define SOUND_FILE URT_DATADIR"sounds/notify1.wav"
#else
#define SOUND_FILE URT_DATADIR"sounds/notify1.ogg"
#endif

enum after_launch_t
{
    al_none = 0,
    al_hide = 1,
    al_quit = 2
};

#define APP_SETTINGS \
        ((start_hidden, bool, false)) \
        ((use_adv_cmd_line, bool, false)) \
        ((use_adv_cmd_line_42, bool, false)) \
        ((adv_cmd_line, QString, QString())) \
        ((adv_cmd_line_42, QString, QString())) \
        ((separate_xsession, bool, true)) \
        ((binary_path, QString, QString())) \
        ((binary_path_42, QString, QString())) \
        ((client_version, QString, QString())) \
        ((geoip_database, QString, QString(URT_DATADIR"GeoIP.dat"))) \
        ((center_current_row, bool, true)) \
        ((keep_history, bool, true)) \
        ((number_in_history, int, 900)) \
        ((days_in_history, int, 90)) \
        ((history_grouping, bool, true)) \
        ((update_before_connect, bool, true)) \
        ((use_holiday_mode, bool, true)) \
        ((clear_offline, bool, true)) \
        ((country_name, QString, QString())) \
        ((style_name, QString, QString())) \
        ((default_style_name, QString, QString())) \
        ((use_mumble_overlay, bool, false)) \
        ((mumble_overlay_bin, QString, QString("/usr/bin/mumble-overlay"))) \
        ((style_sheet_file, QString, QString())) \
        ((notification_sound, QString, QString(SOUND_FILE))) \
        ((multiple_launch, bool, false)) \
        ((after_launch_action, int, 0)) \
        ((install_id, QString, QString()))

SETTINGS_GENERATE_CLASS(app_settings, APP_SETTINGS)

bool is_christmas_mode();

#define CLIP_SETTINGS \
        ((watching, bool, true)) \
        ((regexp, QString, QString("(\\d{1,3}\\.\\d{1,3}\\.\\d{1,3}\\.\\d{1,3})(:(\\d{1,5}))?(.+pass[^\\s]*\\s+([^\\s]+))?"))) \
        ((host, int, 1)) \
        ((port, int, 3)) \
        ((password, int, 5))

SETTINGS_GENERATE_CLASS(clip_settings, CLIP_SETTINGS)

boost::shared_ptr<QTranslator> system_translator(const QString& code);
boost::shared_ptr<QTranslator> local_translator(const QString& code);

#endif
