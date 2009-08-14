#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appoptions.h"
#include "launcher.h"

namespace Ui
{
    class MainWindowClass;
}

class MainWindow : public QMainWindow
{
Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void showOptions();
    void quickConnect();
    void launchStatusChanged();
    void favAdd();
    void favDelete();

private:
    Ui::MainWindowClass *ui;
    AppOptionsPtr opts_;
    Launcher launcher_;
};

#endif
