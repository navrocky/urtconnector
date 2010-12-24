#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <memory>

#include <QMainWindow>
#include <QTreeWidget>

#include "common/server_id.h"

#include "main_tab.h"

#include "pointers.h"

#include <iostream>

class QTreeWidgetItem;

class history_widget : public QMainWindow, public main_tab {
    Q_OBJECT
public:
    history_widget( QWidget *parent, history_p list);
    ~history_widget();
    QTreeWidget* tree() const;
    void update_history();
    int num_rows() const;

    virtual server_id selected_server() const;
    
protected:
    void changeEvent(QEvent *e);

protected Q_SLOTS:
    virtual void servers_updated();

private slots:
    void filter_clear();

private:
    void addItem(history_item_p item);
    ///returns 0 if NO resort needed
    QTreeWidgetItem* add_tem(QTreeWidgetItem* item);
    
    QTreeWidgetItem* find_item( const server_id& id ) const;

    void resort( QTreeWidgetItem* item );
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif // HISTORY_WIDGET_H
