#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>

#include "settings/settings.h"

#include <common/server_bookmark.h>
#include "qstat_options.h"

class QTranslator;

class app_settings: public settings_uid_provider<app_settings>
{
public:
    /*! start with hidden main window */
    bool start_hidden() const;
    void set_start_hidden(bool b);

    /*! Use advanced command line advCmdLine() to launch UrT binary instead of binaryPath(). */
    bool use_adv_cmd_line() const;
    void set_use_adv_cmd_line(bool b);

    /*! Advanced command line to launch UrT binary. Use followed substitutions:
        %host% %port% %user% %password% %rcon% and so on.*/
    QString adv_cmd_line() const;
    void set_adv_cmd_line( const QString& line );

    bool separate_x() const;
    void set_separate_x(bool b);
    
    /*! Path to UrT binary, used when not advanced command line. */
    QString binary_path() const;
    void set_binary_path(const QString& path);

//    /*! Servers options list. */
//    server_fav_list servers;
    
    qstat_settings qstat_opts;

    /*! A path to GeoIP database file */
    QString geoip_database() const;
    void set_geoip_database( const QString& file );

    /*! centering current server row while updating */
    bool center_current_row() const;
    void set_center_current_row(bool b);

    /*! Keep history of connections */
    bool keep_history() const;
    bool set_keep_history(bool b);

    /*! Number of connections that are kept in history. 0 - unlimited. */
    int number_in_history() const;
    void set_number_in_history(int lines );
    
    /*! Days that are kept in history. 0 - unlimited. */
    int days_in_history() const;
    void set_days_in_history(int days);

    bool history_grouping() const;
    void set_history_grouping(bool);

    /*! Update server info before connect to */
    bool update_before_connect() const;
    void set_update_before_connect(bool b);

    /*! Is christmas mode active? */
    bool christmas_mode() const;

    bool use_holiday_mode() const;
    void set_holiday_mode(bool b);

    /*!  Clear offline servers after full refresh*/
    bool clear_offline() const;
    void set_clear_offline(bool b);
    
    /*! Country name to select custom language*/
    QString country_name() const;
    void set_country_name( const QString& country );

    bool use_mumble_overlay() const;
    void set_use_mumble_overlay(bool);
    QString mumble_overlay_bin() const;
    void set_mumble_overlay_bin(const QString&);

    // application style sheet
    void reset_style_sheet_file();
    QString style_sheet_file() const;
    void set_style_sheet_file(const QString&);
};

class clip_settings: public settings_uid_provider<clip_settings>
{
public:
    /*! Look for system clipboard (LFC) */
    bool watching() const;
    void set_watching(bool b);

    /*! LFC regexp */
    QString regexp() const;
    void set_regexp(const QString& rx);
    void reset_regexp();

    /*! LFC regexp match number for host */
    int host() const;
    void set_host(int host);

    /*! LFC regexp match number for port */
    int port() const;
    void set_port(int port);

    /*! LFC regexp match number for password */
    int password() const;
    void set_password(int pass);
};

boost::shared_ptr<QTranslator> system_translator(const QString& code);
boost::shared_ptr<QTranslator> local_translator(const QString& code);

#endif
