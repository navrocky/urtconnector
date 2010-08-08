#ifndef SERVER_LIST_WIDGET_H
#define SERVER_LIST_WIDGET_H

#include <map>

#include <QWidget>
#include <QPointer>
#include <QRegExp>
#include <QStyledItemDelegate>
#include <QTreeWidget>

#include "pointers.h"
#include "server_id.h"
#include "server_options.h"
#include "pointers.h"
#include "filters/pointers.h"

class QLineEdit;
class QToolButton;
class filter_edit_widget;

class server_tree: public QTreeWidget
{
    Q_OBJECT
public:
    server_tree(QWidget* parent);
    QModelIndex indexFromItem(QTreeWidgetItem *item) const;
};

class server_list_widget : public QWidget
{
Q_OBJECT
public:
    server_list_widget(app_options_p opts, filter_factory_p factory, QWidget *parent);
    ~server_list_widget();

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const {return serv_list_;}
    
    /*! Favorites list */
    void set_favs(server_fav_list* favs);

    /*! Access to internal QTreeWidget */
    QTreeWidget* tree() const;

    /*! Current selection in widget */
    server_id_list selection();

    /*! Force update widget */
    void force_update();

private slots:
    void timerEvent(QTimerEvent *event);
    void filter_text_changed(const QString&);
    void filter_clear();
    void edit_filter();
    void update_list();
    
private:
    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    void update_item(QTreeWidgetItem*);
    bool filter_item(QTreeWidgetItem*);

    server_tree* tree_;
    QToolButton* show_filter_button_;
    QLineEdit* filter_edit_;
    QToolButton* clear_filter_button_;
    server_list_p serv_list_;
    server_items items_;
    int old_state_;
    QRegExp filter_rx_;
    int update_timer_;
    int filter_timer_;
    server_fav_list* favs_;
    app_options_p opts_;
    filter_list_p filters_;
    QPointer<filter_edit_widget> edit_widget_;
};

class status_item_delegate : public QStyledItemDelegate
{
public:
    status_item_delegate(QObject* parent = 0);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;
private:
    void next_icon(QRect& icon) const;
};

#endif
