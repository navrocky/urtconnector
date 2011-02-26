#include <QAbstractItemView>
#include <QModelIndex>
#include <QAction>

#include "item_view_dblclick_action_link.h"

item_view_dblclick_action_link::item_view_dblclick_action_link(
    QObject* parent, QAbstractItemView *view, QAction* action)
: QObject(parent)
, a_(action)
{
    connect(view, SIGNAL(doubleClicked(const QModelIndex&)), SLOT(double_clicked(const QModelIndex&)));
}

void item_view_dblclick_action_link::double_clicked(const QModelIndex&)
{
    if (a_ && a_->isEnabled() && a_->isVisible())
        a_->trigger();
}
