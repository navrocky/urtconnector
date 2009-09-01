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
    void serverChanged(const ServerID& id);
    void serverAdded(const ServerID& id);
    void serverRemoved(const ServerID& id);

private:
    void updateItem(ServListItem*);
    Ui_ServListWidgetClass ui_;
    QPointer<ServerListCustom> servList_;
    std::map<ServerID, ServListItem*> items_;
};

#endif
