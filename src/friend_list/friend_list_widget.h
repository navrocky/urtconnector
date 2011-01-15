#ifndef FRIEND_LIST_WIDGET_H
#define FRIEND_LIST_WIDGET_H

#include <memory>

#include <QTreeWidget>

#include "common/server_id.h"

#include "main_tab.h"

#include "pointers.h"

#include <iostream>

class QTreeWidgetItem;

class history_widget : public main_tab
{
    Q_OBJECT
public:
    history_widget(QWidget *parent, history_p list, filter_factory_p factory);
    ~history_widget();
    QTreeWidget* tree() const;
    void update_history();
    int num_rows() const;

    virtual server_id selected_server() const;


    public
Q_SLOTS:
    void delete_selected();


protected:
    void changeEvent(QEvent *e);

    protected
Q_SLOTS:
    virtual void servers_updated();
    virtual void filter_changed();

private slots:
    void filter_clear();

private:
    void addItem(history_item_p item);
    ///returns 0 if NO resort needed
    QTreeWidgetItem* add_item(QTreeWidgetItem* item);

    QTreeWidgetItem* find_item(const server_id& id) const;

    void resort(QTreeWidgetItem* item);

private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif // FRIEND_LIST_WIDGET_H
