#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <boost/shared_ptr.hpp>

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QPointer>

#include "pointers.h"

#include "app_options.h"
#include <launcher/launcher.h>
#include <common/server_list.h>
#include <common/server_bookmark.h>
#include <anticheat/anticheat.h>
#include <jobs/job_queue.h>
#include <geoip/geoip.h>
#include <filters/pointers.h>
#include <history/history_widget.h>
#include <friend_list/friend_list_widget.h>
#include <friend_list/friend_list.h>
#include "clipper.h"

class QTimer;
class Ui_MainWindowClass;
class QSessionManager;
class QAction;
class QProcess;
class QToolBar;
class main_tab_widget;
class main_tab;
class server_list_tab;
class bookmark_tab;

class QAccumulatingConnection;

class main_window : public QMainWindow
{
    Q_OBJECT
public:
    main_window ( QWidget *parent = 0 );
    ~main_window();

private slots:
    void show_options();
    void show_about();
    void show_anticheat_options();
    void quick_connect();
    void quick_add_favorite();
    void connect_selected();
    void update_actions();
    void current_tab_changed();
    void update_server_info();
    void selection_changed();
    void add_selected_to_fav();
    void tray_activated(QSystemTrayIcon::ActivationReason);
    void show_action();
    void quit_action();
    void commit_data_request(QSessionManager&);
    void clipboard_info_obtained();
    void raise_window();

    void open_remote_console();
    
    void clear_all();
    void clear_selected();
    void about_qt();

    void launcher_started();
    void launcher_stopped();

    void save_bookmarks();
    void add_to_friend( const player_info& player );
private:
    void load_all_at_start();
    void save_state_at_exit();
    void save_geometry();
    void load_geometry();
    void update_geoip_database();

    void check_anticheat_prereq() const;
    void connect_to_server(const server_id& id, const QString& player_name, const QString& password);

    server_id selected() const;
    main_tab* current_tab_widget() const;
    server_info_p selected_info() const;

    boost::shared_ptr<Ui_MainWindowClass> ui_;
    main_tab_widget* tab_widget_;
    server_list_tab* all_list_;
    bookmark_tab* fav_list_;
    history_widget* history_list_;
    friend_list_widget* friends_list_;
    
    server_list_p all_sl_;
    server_bookmark_list_p bookmarks_;
    friend_list friends_;
    history_p history_sl_;
    server_id old_id_;
    int old_state_;
    QSystemTrayIcon* tray_;
    QMenu* tray_menu_;
    job_queue* que_;
    geoip gi_;
    clipper* clipper_;
    filter_factory_p filter_factory_;
    anticheat::anticheat* anticheat_;
    launcher* launcher_;
    QAction* anticheat_enabled_action_;
    QAction* anticheat_open_action_;
    QAction* anticheat_configure_action_;
    QPointer<QProcess> ac_proc_;
    QAccumulatingConnection* server_info_updater_;
    QToolBar* tab_toolbar_;
};

#endif
