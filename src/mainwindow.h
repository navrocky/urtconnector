#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "appoptions.h"

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
    
private:
    Ui::MainWindowClass *ui;
    AppOptionsPtr opts_;
};

#endif
