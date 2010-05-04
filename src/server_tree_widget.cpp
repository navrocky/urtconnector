
#include "server_tree_widget.h"


server_tree_widget::server_tree_widget(QWidget* p)
    :QTreeWidget(p)
{}

server_tree_widget::~server_tree_widget()
{}

QModelIndex server_tree_widget::indexFromItem(QTreeWidgetItem* item) const
{ return QTreeWidget::indexFromItem(item); }
