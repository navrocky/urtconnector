#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <memory>
#include <QMainWindow>

#include "ui_mainwindow.h"
#include "appoptions.h"
#include "launcher.h"
#include "servlistwidget.h"
#include "serverlistcustom.h"

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void saveOptions();
    void loadOptions();

private slots:
    void showOptions();
    void quickConnect();
    void launchStatusChanged();
    void favAdd();
    void favDelete();
    void refreshAll();

private:
    void syncFavList();

    Ui::MainWindowClass ui;
    AppOptionsPtr opts_;
    Launcher launcher_;
    ServListWidget* allList_;
    ServListWidget* favList_;
    ServerListCustom* allSL_;
    ServerListCustom* favSL_;
};

#endif
