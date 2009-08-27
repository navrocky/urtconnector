#ifndef SERVLISTWIDGET_H
#define SERVLISTWIDGET_H

#include <QWidget>

#include "ui_servlistwidget.h"


class ServListWidget : public QWidget
{
Q_OBJECT
public:
    ServListWidget(QWidget *parent = 0);
    ~ServListWidget();

private:
    Ui_ServListWidgetClass ui_;
};

#endif
