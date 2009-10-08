#ifndef SERVLISTWIDGET_H
#define SERVLISTWIDGET_H

#include <map>
#include <QWidget>
#include <QPointer>
#include <QRegExp>

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

    QTreeWidget* tree() const {return ui_.treeWidget;}

private slots:
    void timerEvent(QTimerEvent *event);
    void filterTextChanged(const QString&);
private:
    void updateItem(ServListItem*);
    void updateList();
    bool filterItem(ServListItem*);
    Ui_ServListWidgetClass ui_;
    QPointer<ServerListCustom> servList_;

    typedef std::map<ServerID, ServListItem*> ServItems;
    ServItems items_;

    int oldState_;
    QRegExp filterRx_;
    int updateTimer_;
    int filterTimer_;
};

#endif
