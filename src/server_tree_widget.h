
#ifndef URT_SERVER_TREE_WIDGET
#define URT_SERVER_TREE_WIDGET

#include <QTreeWidget>

class server_tree_widget: public QTreeWidget{
    Q_OBJECT
public:
    server_tree_widget(QWidget* parent);
    ~server_tree_widget();

//    QModelIndex index( QTreeWidgetItem* item ) const;
    QModelIndex indexFromItem(QTreeWidgetItem *item) const;
};

#endif

