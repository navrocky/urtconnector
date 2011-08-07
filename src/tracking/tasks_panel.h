#ifndef TASKS_PANEL_H
#define	TASKS_PANEL_H

#include <memory>
#include <QWidget>
#include <QMap>

#include "pointers.h"

class Ui_tasks_panel;
class QTreeWidgetItem;

namespace tracking
{

class manager;
class task_t;

class tasks_panel : public QWidget
{
    Q_OBJECT
public:
    tasks_panel(manager* man, 
                const condition_factory_p& conds,
                const action_factory_p& acts,
                QWidget* parent);

    bool event(QEvent*);

private slots:
    void update_tasks();

    void create_task();
    void edit_task();
    void delete_task();
    void execute_task(bool);

    void update_actions();

private:
    void update_item(QTreeWidgetItem*);

    std::auto_ptr<Ui_tasks_panel> ui_;
    manager* man_;
    condition_factory_p conds_;
    action_factory_p acts_;
    QMap<task_t*, QTreeWidgetItem*> items_map_;
};

}

#endif	/* TASKS_PANEL_H */

