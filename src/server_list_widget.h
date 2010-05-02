#ifndef SERVER_LIST_WIDGET_H
#define SERVER_LIST_WIDGET_H

#include <map>

#include <QWidget>
#include <QPointer>
#include <QRegExp>

#include "ui_server_list_widget.h"
#include "pointers.h"
#include "server_id.h"
#include "geoip/geoip.h"

class server_list_item;

class server_list_widget : public QWidget
{
Q_OBJECT
public:
    server_list_widget(QWidget *parent, const geoip& gi );
    ~server_list_widget();

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const {return serv_list_;}

    QTreeWidget* tree() const {return ui_.treeWidget;}

    /*! Current selection in widget */
    server_id_list selection();

    void force_update();

private slots:
    void timerEvent(QTimerEvent *event);
    void filter_text_changed(const QString&);
    void filter_clear();
private:
    void update_item(server_list_item*);
    void update_list();
    bool filter_item(server_list_item*);

    Ui_ServListWidgetClass ui_;
    server_list_p serv_list_;

    typedef std::map<server_id, server_list_item*> server_items;
    server_items items_;

    int old_state_;
    QRegExp filter_rx_;
    int update_timer_;
    int filter_timer_;
    geoip   gi_;
};

#endif