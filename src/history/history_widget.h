#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <memory>

#include <QMainWindow>
#include <QTreeWidget>

#include <common/server_id.h>

#include "pointers.h"

class QTreeWidgetItem;

class history_widget : public QMainWindow {
    Q_OBJECT
public:
    history_widget( QWidget *parent, history_p list);
    ~history_widget();
    QTreeWidget* tree() const;
    void update_history();
    int num_rows() const;

    server_id current_server() const;

    void set_server_list(server_list_p ptr);
    server_list_p server_list() const;
    
protected:
    void changeEvent(QEvent *e);

private slots:
    void filter_clear();

private:
    void addItem(history_item_p item);
    QTreeWidgetItem* find_item( const server_id& id ) const;
    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif // HISTORY_WIDGET_H
