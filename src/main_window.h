#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>

#include <QMainWindow>
#include <QSystemTrayIcon>

#include "pointers.h"

#include "app_options.h"
#include "launcher.h"
#include "server_list_widget.h"
#include "server_list.h"
#include "jobs/job_queue.h"
#include "geoip/geoip.h"
#include "clipper.h"

class QTimer;
class Ui_MainWindowClass;


class tab_size_updater: public QObject
{
    Q_OBJECT
public:
    tab_size_updater ( QTabWidget* tw, int index );
    ~tab_size_updater();

public Q_SLOTS:
    void update_size ( int i ) const;
private:
    QTabWidget* tw_;
    int index_;
};

class main_window : public QMainWindow
{
    Q_OBJECT
public:
    main_window ( QWidget *parent = 0 );
    ~main_window();

protected:
    void closeEvent ( QCloseEvent *event );

private slots:
    void show_options();
    void show_about();
    void quick_connect() const;
    void connect_selected() const;
    void fav_add();
    void fav_edit();
    void fav_delete();
    void refresh_all();
    void refresh_selected();
    void update_actions();
    void current_tab_changed ( int );
    void update_server_info();
    void selection_changed();
    void add_selected_to_fav();
    void tray_activated ( QSystemTrayIcon::ActivationReason reason );
    void show_action();
    void quit_action();
private:
    void sync_fav_list();

    void save_geometry(settings s);
    void load_geometry(settings s);

    server_id selected() const;
    server_list_widget* selected_list_widget() const;
    server_info_p selected_info() const;

    std::auto_ptr<Ui_MainWindowClass> ui_;
    app_options_p opts_;
    server_list_widget* all_list_;
    server_list_widget* fav_list_;
    server_list_p all_sl_;
    server_list_p fav_sl_;
    QTimer* serv_info_update_timer_;
    server_id old_id_;
    int old_state_;
    QSystemTrayIcon* tray_;
    QMenu* tray_menu_;
    job_queue* que_;
    geoip gi_;
    clipper* clipper_;
};

#endif
