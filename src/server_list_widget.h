#ifndef SERVER_LIST_WIDGET_H
#define SERVER_LIST_WIDGET_H

#include <map>

#include <QWidget>
#include <QPointer>
#include <QRegExp>
#include <QStyledItemDelegate>

#include "ui_server_list_widget.h"
#include "pointers.h"
#include "server_id.h"
#include "server_options.h"
#include "pointers.h"

class server_list_item;

class server_list_widget : public QWidget
{
Q_OBJECT
public:
    server_list_widget(app_options_p opts, QWidget *parent);

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const {return serv_list_;}
    
    /*! Favorites list */
    void set_favs(server_fav_list* favs);

    /*! Access to internal QTreeWidget */
    QTreeWidget* tree() const {return ui_.treeWidget;}

    /*! Current selection in widget */
    server_id_list selection();

    /*! Force update widget */
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
    server_fav_list* favs_;
    app_options_p opts_;
};


class status_item_delegate : public QStyledItemDelegate{
public:
        
    status_item_delegate(QObject* parent = 0);
    virtual ~status_item_delegate();

    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
    void next_icon(QRect& icon) const;
};

#endif
