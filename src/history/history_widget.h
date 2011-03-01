#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <common/server_id.h>
#include <tabs/filtered_tab.h>

#include "../pointers.h"

class QTreeWidget;
class QTreeWidgetItem;
class history_item;

class history_widget : public filtered_tab
{
    Q_OBJECT
public:
    history_widget(history_p history,
                   const tab_context& ctx,
                   QWidget *parent);


//    int num_rows() const;

    virtual server_id selected_server() const;

public slots:
    void delete_selected();

protected slots:
//    virtual void servers_updated();
    virtual void filter_changed();

private:
    void update_contents();

    void addItem(const history_item& item);

    ///returns 0 if NO resort needed
    QTreeWidgetItem* add_item(QTreeWidgetItem* item);

    QTreeWidgetItem* find_item(const server_id& id) const;

    void resort(QTreeWidgetItem* item);

private:
    QTreeWidget* tree_;
    history_p history_;
};

#endif // HISTORY_WIDGET_H
