#ifndef FILTER_EDIT_WIDGET_H
#define	FILTER_EDIT_WIDGET_H

#include <QWidget>

#include "pointers.h"

class QTreeWidget;
class QTreeWidgetItem;
class QAction;

class composite_filter;

class filter_item_widget : public QWidget
{
    Q_OBJECT
public:
    filter_item_widget(filter_p filter, QWidget* parent);
    void set_selected(bool val);

private:
    void update_selected();
    bool selected_;
    filter_p filter_;
};


class filter_edit_widget : public QWidget
{
    Q_OBJECT
public:
    filter_edit_widget(filter_list_p filters, QWidget* parent = NULL);

private:
    void do_update(filter_p parent, QTreeWidget*, QTreeWidgetItem*);
    void update_item(QTreeWidgetItem*);

    void update_contents();
    composite_filter* composite_cast(filter_p);

    filter_list_p filters_;
    QTreeWidget* tree_;
    QAction* add_new_filter_action_;
    QAction* add_exist_filter_action_;
};

#endif	/* FILTER_EDIT_WIDGET_H */

