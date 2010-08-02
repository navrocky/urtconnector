#ifndef FILTER_EDIT_WIDGET_H
#define	FILTER_EDIT_WIDGET_H

#include <QWidget>

#include "pointers.h"

class QTreeWidget;
class QAction;

class filter_edit_widget : public QWidget
{
    Q_OBJECT
public:
    filter_edit_widget(filter_list_p filters, QWidget* parent = NULL);

private:
    filter_list_p filters_;
    QTreeWidget* tree_;
    QAction* add_new_filter_action_;
    QAction* add_exist_filter_action_;
};

#endif	/* FILTER_EDIT_WIDGET_H */

