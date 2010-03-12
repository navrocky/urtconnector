#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>

#include "ui_mainwindow.h"
#include "appoptions.h"
#include "launcher.h"
#include "servlistwidget.h"
#include "serverlistcustom.h"

class main_window : public QMainWindow
{
Q_OBJECT
public:
    main_window(QWidget *parent = 0);
    ~main_window();

public slots:
    void save_options();
    void load_options();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void show_options();
    void show_about();
    void quick_connect();
    void connect_selected();
    void launch_status_changed();
    void fav_add();
    void fav_edit();
    void fav_delete();
    void refresh_all();
    void refresh_all_stopped();
    void refresh_selected();
    void update_actions();
    void current_tab_changed(int);

private:
    void sync_fav_list();

    void save_geometry();
    void load_geometry();

    server_id selected();
    serv_list_widget* selected_list_widget();

    Ui::MainWindowClass ui;
    app_options_ptr opts_;
    launcher launcher_;
    serv_list_widget* all_list_;
    serv_list_widget* fav_list_;
    serv_list_custom* all_sl_;
    serv_list_custom* fav_sl_;
};

#endif
