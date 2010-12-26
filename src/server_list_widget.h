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
#include <filters/pointers.h>
#include <settings/settings.h>

#include "pointers.h"
#include "server_bookmark.h"

class QLineEdit;
class QToolButton;
class QDockWidget;
class filter_edit_widget;

class server_tree: public QTreeWidget
{
    Q_OBJECT
public:
    server_tree(QWidget* parent);
    QModelIndex indexFromItem(QTreeWidgetItem *item) const;
};

class server_list_widget_settings: public settings_uid_provider<server_list_widget_settings>
{
public:
    server_list_widget_settings(const QString& list_name);
    filter_p load_root_filter(filter_factory_p factory);
    void save_root_filter(filter_p f);

    QString load_toolbar_filter();
    void save_toolbar_filter(const QString&);

    void save_state(const QByteArray& a);
    QByteArray load_state();

    bool is_filter_visible();
    void set_filter_visible(bool val);
private:
    QString name_;
};

class server_list_widget : public QMainWindow
{
Q_OBJECT
public:
    server_list_widget( filter_factory_p factory, QWidget *parent);
    ~server_list_widget();

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const {return serv_list_;}
    
    /*! Bookmark list */
    void set_bookmarks(server_bookmark_list* bms);

    /*! Access to internal QTreeWidget */
    QTreeWidget* tree() const;

    /*! Current selection in widget */
    server_id_list selection();

    /*! Force update widget */
    void force_update();

    void load_options();
    void save_options();

    /*! Visible server count passed through filters */
    int visible_server_count() const {return visible_server_count_;}

    bool eventFilter(QObject* watched, QEvent* event);

private slots:
    void edit_filter();
    void update_list();
    void update_toolbar_filter();
    
private:
    typedef std::map<server_id, QTreeWidgetItem*> server_items;

    void update_item(QTreeWidgetItem*);
    bool filter_item(QTreeWidgetItem*);

    server_tree* tree_;
    server_list_p serv_list_;
    server_items items_;
    QPointer<server_bookmark_list> bms_;
    filter_list_p filters_;
    QDockWidget* filter_widget_;
    int visible_server_count_;
    QWidget* filter_holder_;
    filter_edit_widget* filter_edit_widget_;
    QAccumulatingConnection* accum_updater_;
    QAction* show_filter_action_;
};

class status_item_delegate : public QStyledItemDelegate
{
public:
    status_item_delegate(QObject* parent = 0);
    status_item_delegate(server_list_p sl, QObject* parent = 0);
    virtual void paint(QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const;
private:
    void next_icon(QRect& icon) const;
    server_list_p sl_;    
};

#endif
