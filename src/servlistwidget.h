#ifndef SERVLISTWIDGET_H
#define SERVLISTWIDGET_H

#include <QWidget>
#include <QPointer>

#include "ui_servlistwidget.h"

#include "serverlistcustom.h"

class ServListWidget : public QWidget
{
Q_OBJECT
public:
    ServListWidget(QWidget *parent = 0);
    ~ServListWidget();

    void setServerList(ServerListCustom* ptr);

private:
    Ui_ServListWidgetClass ui_;
    QPointer<ServerListCustom> servList_;
};

#endif
