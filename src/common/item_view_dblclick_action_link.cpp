#include <QAbstractItemView>
#include <QModelIndex>
#include <QAction>

#include "item_view_dblclick_action_link.h"

item_view_dblclick_action_link::item_view_dblclick_action_link(
    QObject* parent, QAbstractItemView *view, QAction* action)
: QObject(parent)
{
    connect(view, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(double_clicked(const QModelIndex&)));
    connect(this, SIGNAL(triggered()), action, SLOT(trigger()));
}

void item_view_dblclick_action_link::double_clicked(const QModelIndex&)
{
    emit triggered();
}
