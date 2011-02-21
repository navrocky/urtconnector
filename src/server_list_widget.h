#ifndef SERVER_LIST_WIDGET_H
#define SERVER_LIST_WIDGET_H

#include <map>

#include <QMainWindow>
#include <QWidget>
#include <QPointer>
#include <QRegExp>
#include <QStyledItemDelegate>
#include <QTreeWidget>

#include <common/server_id.h>
#include <common/qaccumulatingconnection.h>
#include <tabs/filtered_tab.h>
#include <filters/pointers.h>
#include <settings/settings.h>

#include "pointers.h"
#include <common/server_bookmark.h>
#include "tabs/filtered_tab.h"

class QLineEdit;
class QToolButton;
class QDockWidget;
class filter_edit_widget;

class server_tree : public QTreeWidget
{
    Q_OBJECT
public:
    server_tree(QWidget* parent);
    QModelIndex indexFromItem(QTreeWidgetItem *item) const;
};

//class status_item_delegate : public QStyledItemDelegate
//{
//public:
//    status_item_delegate(server_list_p sl, QObject* parent = 0);
//    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
//                       const QModelIndex& index) const;
//
//    void set_server_list(const server_list_p& sl)
//    {
//        if (sl != sl_)
//            sl_ = sl;
//    }
//
//    virtual QSize sizeHint(const QStyleOptionViewItem & option, const QModelIndex & index) const;
//
//private:
//    void next_icon(QRect& icon) const;
//    server_list_p sl_;
//};

class server_list_tab : public filtered_tab
{
    Q_OBJECT
public:
    server_list_tab(const QString& object_name, server_list_p serv_list, filter_factory_p factory, QWidget *parent);
    ~server_list_tab();

    //     void set_server_list(server_list_p ptr);
    //     server_list_p server_list() const {return serv_list_;}

    /*! Bookmark list */
    void set_bookmarks(server_bookmark_list* bms);

    /*! Access to internal QTreeWidget */
    QTreeWidget* tree() const;

    /*! Current selection in widget */
    virtual server_id_list selection() const;

    //     void load_options();
    //     void save_options();

    /*! Visible server count passed through filters */
    int visible_server_count() const
    {
        return visible_server_count_;
    }

    //     bool eventFilter(QObject* watched, QEvent* event);

    protected
Q_SLOTS:

    ///this function automatically called when server_list has some changes
    virtual void servers_updated();
    virtual void filter_changed();

private:

    void update_list();

    void update_item(QTreeWidgetItem*);

private:
    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    QTreeWidget* tree_;

    QPointer<server_bookmark_list> bms_;

    server_items items_;
    int visible_server_count_;
};


typedef server_list_tab server_list_widget;


class server_list_widget_settings : public filtered_tab_settings
{
public:
    server_list_widget_settings(const QString& object_name);
};

#endif
