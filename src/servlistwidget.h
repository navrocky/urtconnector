#ifndef SERVLISTWIDGET_H
#define SERVLISTWIDGET_H

#include <map>
#include <QWidget>
#include <QPointer>
#include <QRegExp>

#include "ui_servlistwidget.h"

#include "serverlistcustom.h"

class ServListItem;

class serv_list_widget : public QWidget
{
Q_OBJECT
public:
    serv_list_widget(QWidget *parent = 0);
    ~serv_list_widget();

    void setServerList(serv_list_custom* ptr);
    serv_list_custom* serverList() const {return servList_;}

    QTreeWidget* tree() const {return ui_.treeWidget;}

    // current selection
    server_id_list selection();

    void forceUpdate();

private slots:
    void timerEvent(QTimerEvent *event);
    void filterTextChanged(const QString&);
    void filter_clear();
private:
    void updateItem(ServListItem*);
    void updateList();
    bool filterItem(ServListItem*);
    Ui_ServListWidgetClass ui_;
    QPointer<serv_list_custom> servList_;

    typedef std::map<server_id, ServListItem*> ServItems;
    ServItems items_;

    int oldState_;
    QRegExp filterRx_;
    int updateTimer_;
    int filterTimer_;
};

#endif
