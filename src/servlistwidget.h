#ifndef SERVLISTWIDGET_H
#define SERVLISTWIDGET_H

#include <map>
#include <QWidget>
#include <QPointer>

#include "ui_servlistwidget.h"

#include "serverlistcustom.h"

class ServListItem;

class ServListWidget : public QWidget
{
Q_OBJECT
public:
    ServListWidget(QWidget *parent = 0);
    ~ServListWidget();

    void setServerList(ServerListCustom* ptr);

private slots:
    void timerEvent(QTimerEvent *event);
private:
    void updateItem(ServListItem*);
    void updateList();
    Ui_ServListWidgetClass ui_;
    QPointer<ServerListCustom> servList_;

    typedef std::map<ServerID, ServListItem*> ServItems;
    ServItems items_;

    int oldState_;
};

#endif
