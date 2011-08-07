#include "tasks_panel.h"

#include <boost/bind.hpp>

#include <QTreeWidgetItem>
#include <QUuid>

#include <common/tree_smart_updater.h>
#include <common/tools.h>
#include <common/qt_syslog.h>
#include <common/item_view_dblclick_action_link.h>
#include <qnamespace.h>
#include "manager.h"
#include "task.h"
#include "condition.h"
#include "task_prop_dlg.h"

#include "ui_tasks_panel.h"

Q_DECLARE_METATYPE(tracking::task_t*)

SYSLOG_MODULE(tasks_panel)

namespace tracking
{

const int c_task_role = Qt::UserRole;

tasks_panel::tasks_panel(manager* man, 
                         const condition_factory_p& conds,
                         const action_factory_p& acts,
                         QWidget* parent)
: QWidget(parent)
, ui_(new Ui_tasks_panel)
, man_(man)
, conds_(conds)
, acts_(acts)
{
    ui_->setupUi(this);
    ui_->add_task_btn->setDefaultAction(ui_->create_task_action);
    connect(ui_->create_task_action, SIGNAL(triggered()), SLOT(create_task()));
    ui_->edit_task_btn->setDefaultAction(ui_->edit_task_action);
    connect(ui_->edit_task_action, SIGNAL(triggered()), SLOT(edit_task()));
    ui_->del_task_btn->setDefaultAction(ui_->delete_task_action);
    connect(ui_->delete_task_action, SIGNAL(triggered()), SLOT(delete_task()));
    ui_->execute_task_btn->setDefaultAction(ui_->execute_task_action);
    connect(ui_->execute_task_action, SIGNAL(toggled(bool)), SLOT(execute_task(bool)));

    new item_view_dblclick_action_link(this, ui_->tasks_list, ui_->execute_task_action);

    QTreeWidget* tree = ui_->tasks_list;
    tree->addAction(ui_->create_task_action);
    tree->addAction(ui_->edit_task_action);
    tree->addAction(ui_->delete_task_action);
    add_separator_action(tree);
    tree->addAction(ui_->execute_task_action);
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(man_, SIGNAL(changed()), SLOT(update_tasks()));

    connect(ui_->tasks_list, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)), SLOT(update_actions()));
}

bool tasks_panel::event(QEvent* e)
{
    if (e->type() == QEvent::Show)
    {
        update_tasks();
    }

    return QWidget::event(e);
}

void tasks_panel::update_tasks()
{
    if (!isVisible())
        return;

    LOG_DEBUG << "Update task list";
    const manager::task_list_t& tl = man_->tasks();
    smart_update_tree_contents( tl, c_task_role, ui_->tasks_list, 0,
        boost::bind(&tasks_panel::update_item, this, _1), items_map_);
    
    update_actions();
}

void tasks_panel::update_item(QTreeWidgetItem* item)
{
    task_t* t = item->data(0, c_task_role).value<task_t*>();
    item->setText(0, t->caption());

    if (t->condition() && t->condition()->is_started())
        item->setIcon(0, QIcon("icons:media-playback-start.png"));
    else
        item->setIcon(0, QIcon());
    if (t->condition())
    {
//        t->condition()->is_started();
    }
}

void tasks_panel::create_task()
{
    std::auto_ptr<task_t> t(new task_t(this));
    QUuid uid = QUuid::createUuid();
    t->set_id(uid.toString());
    t->set_caption(tr("New task"));

    task_prop_dlg d(t.get(), conds_, acts_, this);
    d.setWindowTitle(tr("Create new task"));
    if (d.exec() != QDialog::Accepted)
        return;

    man_->add_task(t.release());
}

void tasks_panel::delete_task()
{
    QTreeWidgetItem* item = ui_->tasks_list->currentItem();
    task_t* t = item->data(0, c_task_role).value<task_t*>();
    delete t;
}

void tasks_panel::execute_task(bool new_val)
{
    QTreeWidgetItem* item = ui_->tasks_list->currentItem();
    if (!item)
        return;
    task_t* t = item->data(0, c_task_role).value<task_t*>();
    bool old_val = t->condition()->is_started();
    if (new_val == old_val)
        return;
    if (new_val)
        t->condition()->start();
    else
        t->condition()->stop();
}

void tasks_panel::update_actions()
{
    QTreeWidgetItem* item = ui_->tasks_list->currentItem();
    task_t* t = item ? item->data(0, c_task_role).value<task_t*>() : 0;
    ui_->delete_task_action->setEnabled(item);
    ui_->edit_task_action->setEnabled(item);
    ui_->execute_task_action->setEnabled(item);
    ui_->execute_task_action->setChecked(t && t->condition() && t->condition()->is_started());
}

void tasks_panel::edit_task()
{
    QTreeWidgetItem* item = ui_->tasks_list->currentItem();
    task_t* t = item->data(0, c_task_role).value<task_t*>();

    task_t tmp_task;
    tmp_task.assign(t);

    task_prop_dlg d(&tmp_task, conds_, acts_, this);
    if (d.exec() != QDialog::Accepted)
        return;

    t->assign(&tmp_task);
    update_tasks();
}

}

