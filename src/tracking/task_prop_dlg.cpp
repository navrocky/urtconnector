#include "task_prop_dlg.h"

#include <cassert>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QComboBox>
#include <QToolButton>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QLabel>

#include <common/scoped_tools.h>
#include <common/tools.h>

#include "task.h"
#include "condition.h"
#include "action.h"

#include "ui_task_prop_dlg.h"

Q_DECLARE_METATYPE(tracking::condition_class_p)
Q_DECLARE_METATYPE(tracking::action_class_p)
Q_DECLARE_METATYPE(tracking::task_t::operation_mode_t)

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// action_widget

action_widget::action_widget(QTreeWidgetItem* item,
                             action_factory_p factory,
                             action_p action,
                             QWidget* parent)
: QWidget(parent)
, item_(item)
, factory_(factory)
, action_(action)
, block_action_combo_(false)
{
    QVBoxLayout* l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    new_action_panel_ = new QWidget(this);
    l->addWidget(new_action_panel_);

    QGridLayout* gl = new QGridLayout(new_action_panel_);

    QLabel* lab = new QLabel(new_action_panel_);
    lab->setText(tr("Select new action"));

    int size = style()->pixelMetric(QStyle::PM_ButtonIconSize);

    lab->setPixmap(QIcon("icons:add.png").pixmap(size));
    lab->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    gl->addWidget(lab, 0, 0);

    action_select_combo_ = new QComboBox(new_action_panel_);
    connect(action_select_combo_, SIGNAL(activated(int)), SLOT(combo_activated(int)));
    gl->addWidget(action_select_combo_, 0, 1);

    cancel_btn_ = new QToolButton(new_action_panel_);
    cancel_btn_->setIcon(QIcon("icons:close.png"));
    cancel_btn_->setAutoRaise(true);
    cancel_btn_->setToolTip(tr("Cancel new action creation"));
    gl->addWidget(cancel_btn_, 0, 2);
    connect(cancel_btn_, SIGNAL(clicked()), SLOT(delete_action()));

    action_panel_holder_ = new QWidget(this);
    l->addWidget(action_panel_holder_);

    QVBoxLayout* vl = new QVBoxLayout(action_panel_holder_);

    QHBoxLayout* hl = new QHBoxLayout;
    hl->setContentsMargins(0, 0, 0, 0);
    vl->addLayout(hl);
    header_icon_ = new QLabel(action_panel_holder_);
    header_icon_->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    hl->addWidget(header_icon_);
    header_label_ = new QLabel(action_panel_holder_);
    hl->addWidget(header_label_);
    delete_btn_ = new QToolButton(action_panel_holder_);
    delete_btn_->setIcon(QIcon("icons:close.png"));
    delete_btn_->setAutoRaise(true);
    delete_btn_->setToolTip(tr("Delete this action"));
    hl->addWidget(delete_btn_);
    connect(delete_btn_, SIGNAL(clicked()), SLOT(delete_action()));

    update_contents();
}

void action_widget::activate_select()
{
    if (action_)
        return;
    action_select_combo_->showPopup();
}

void action_widget::update_contents()
{
    delete action_panel_;
    if (action_)
    {
        new_action_panel_->hide();
        action_panel_holder_->show();
        action_panel_ = action_->create_options_widget(action_panel_holder_);
        int size = style()->pixelMetric(QStyle::PM_ButtonIconSize);
        action_class_p ac = action_->get_class();
        header_icon_->setPixmap(ac->icon().pixmap(size));
        header_label_->setText(ac->caption());
        static_cast<QBoxLayout*>(action_panel_holder_->layout())->addWidget(action_panel_);
        action_panel_->setFocus();
        item_->setHidden(true);
        item_->setHidden(false);
        setToolTip(ac->description());
    } else
    {
        SCOPE_COCK_FLAG(block_action_combo_);
        action_panel_holder_->hide();
        new_action_panel_->show();
        action_select_combo_->clear();
        action_select_combo_->addItem(tr("... Select new action ..."));

        foreach (action_factory::classes_t::const_reference r, factory_->classes())
        {
            const action_class_p& ac = r.second;
            action_select_combo_->addItem(ac->icon(), ac->caption(), QVariant::fromValue(ac));
        }
    }
}

void action_widget::combo_activated(int i)
{
    if (block_action_combo_)
        return;
    action_class_p ac = action_select_combo_->itemData(i).value<action_class_p>();
    if (!ac)
        return;
    
    action_ = ac->create();
    update_contents();

    emit action_created();
}

void action_widget::delete_action()
{
    emit action_canceled();
}


////////////////////////////////////////////////////////////////////////////////
// task_prop_dlg

task_prop_dlg::task_prop_dlg(task_t* t,
                             const condition_factory_p& conds,
                             const action_factory_p& acts,
                             QWidget* parent)
: QDialog(parent)
, ui_(new Ui_task_prop_dlg)
, task_(t)
, conds_(conds)
, acts_(acts)
, new_action_item_(0)
, updating_active_(false)
{
    assert(!t->id().isEmpty());
    ui_->setupUi(this);
    ui_->mode_combo->addItem(tr("Multi trigger"), QVariant::fromValue(int(task_t::om_multi_trigger)));
    ui_->mode_combo->addItem(tr("Single trigger"), QVariant::fromValue(int(task_t::om_single_trigger)));
    ui_->mode_combo->addItem(tr("Destroy after trigger"), QVariant::fromValue(int(task_t::om_destroy_after_trigger)));
    connect(ui_->mode_combo, SIGNAL(currentIndexChanged(int)), SLOT(mode_combo_changed()));

    ui_->add_action_btn->setDefaultAction(ui_->create_new_action);
    ui_->del_action_btn->setDefaultAction(ui_->delete_selected_action);

    connect(ui_->cond_combo, SIGNAL(currentIndexChanged(int)), SLOT(cond_combo_changed()));
    connect(ui_->create_new_action, SIGNAL(triggered()), SLOT(add_action()));
    connect(ui_->delete_selected_action, SIGNAL(triggered()), SLOT(delete_selected_action()));
    connect(ui_->actions_list, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)), SLOT(update_actions()));
    
    QVBoxLayout* lay = new QVBoxLayout(ui_->cond_holder_widget);
    lay->setContentsMargins(0, 0, 0, 0);

    QList<QAction*> al;
    al << ui_->create_new_action << ui_->delete_selected_action;
    ui_->actions_list->addActions(al);
    ui_->actions_list->setContextMenuPolicy(Qt::ActionsContextMenu);
    update_contents();
    ui_->splitter->setStretchFactor(0, 100);
    ui_->splitter->setStretchFactor(1, 100);
}

void task_prop_dlg::accept()
{
    QDialog::accept();
    task_->set_caption(ui_->name_edit->text());
}

void task_prop_dlg::update_contents()
{
    SCOPE_COCK_FLAG(updating_active_);

    // update mode combo
    int i = ui_->mode_combo->findData(QVariant(int(task_->operation_mode())));
    ui_->mode_combo->setCurrentIndex(i);

    // update condition combo
    ui_->cond_combo->clear();
    foreach (condition_factory::classes_t::const_reference r, conds_->classes())
    {
        const condition_class_p& cc = r.second;
        ui_->cond_combo->addItem(cc->icon(), cc->caption(), cc->id());
    }

    condition_p cond = task_->condition();
    if (!cond)
    {
        // creating first condition
        condition_factory::classes_t::const_iterator it = conds_->classes().begin();
        assert(it != conds_->classes().end());
        cond = it->second->create();
        task_->set_condition(cond);
    }

    i = ui_->cond_combo->findData(cond->get_class()->id());
    if (i >= 0)
        ui_->cond_combo->setCurrentIndex(i);

    ui_->cond_combo->setToolTip(cond->get_class()->description());
    QBoxLayout* lay = qobject_cast<QBoxLayout*>(ui_->cond_holder_widget->layout());
    cond_widget_ = cond->create_options_widget(ui_->cond_holder_widget);
    lay->addWidget(cond_widget_);

    ui_->name_edit->setText(task_->caption());

    QTreeWidget* tree = ui_->actions_list;
    foreach (const action_p& a, task_->actions())
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(tree);
        action_widget* aw = new action_widget(item, acts_, a, this);
        tree->setItemWidget(item, 0, aw);
        connect(aw, SIGNAL(action_canceled()), SLOT(action_canceled()));
        connect(aw, SIGNAL(action_created()), SLOT(new_action_created()));
    }

    update_actions();
}

void task_prop_dlg::cond_combo_changed()
{
    if (updating_active_)
        return;
    delete cond_widget_;
    const QString& cc_id = ui_->cond_combo->itemData(ui_->cond_combo->currentIndex()).toString();
    condition_class_p cc = conds_->get(cc_id);
    ui_->cond_combo->setToolTip(cc->description());

    condition_p c = cc->create();
    task_->set_condition(c);
    QBoxLayout* lay = qobject_cast<QBoxLayout*>(ui_->cond_holder_widget->layout());
    cond_widget_ = c->create_options_widget(ui_->cond_holder_widget);
    lay->addWidget(cond_widget_);
}

void task_prop_dlg::add_action()
{
    if (new_action_item_)
        return;
    QTreeWidget* tree = ui_->actions_list;
    new_action_item_ = new QTreeWidgetItem(tree);

    action_widget* aw = new action_widget(new_action_item_, acts_, action_p(), this);
    tree->setItemWidget(new_action_item_, 0, aw);

    QTimer::singleShot(100, aw, SLOT(activate_select()));
    tree->scrollToItem(new_action_item_);
    tree->setCurrentItem(new_action_item_);
    connect(aw, SIGNAL(action_canceled()), SLOT(action_canceled()));
    connect(aw, SIGNAL(action_created()), SLOT(new_action_created()));
    ui_->add_action_btn->setEnabled(false);
    update_actions();
}

void task_prop_dlg::action_canceled()
{
    action_widget* aw = qobject_cast<action_widget*>(sender());
    if (aw->item() != new_action_item_)
    {
        task_->remove_action(aw->current_action());
        new_action_item_ = aw->item();
    }
    QTimer::singleShot(0, this, SLOT(delete_action()));
}

void task_prop_dlg::delete_action()
{
    ui_->add_action_btn->setEnabled(true);
    delete new_action_item_;
    new_action_item_ = 0;
}

void task_prop_dlg::new_action_created()
{
    ui_->add_action_btn->setEnabled(true);
    task_->add_action(get_action_from_item(new_action_item_));
    new_action_item_ = 0;
    update_actions();
}

action_p task_prop_dlg::get_action_from_item(QTreeWidgetItem* item)
{
    if (item)
    {
        action_widget* aw = qobject_cast<action_widget*>(ui_->actions_list->itemWidget(item, 0));
        return aw->current_action();
    } else
        return action_p();
}

void task_prop_dlg::update_actions()
{
    QTreeWidgetItem* item = ui_->actions_list->currentItem();
    action_p a = get_action_from_item(item);
    ui_->del_action_btn->setEnabled(a);
}

void task_prop_dlg::delete_selected_action()
{
    QTreeWidgetItem* item = ui_->actions_list->currentItem();
    action_p a = get_action_from_item(item);
    task_->remove_action(a);
    delete item;
}

void task_prop_dlg::mode_combo_changed()
{
    if (updating_active_)
        return;
    task_->set_operation_mode(task_t::operation_mode_t(ui_->mode_combo->itemData(
        ui_->mode_combo->currentIndex()).toInt()));
}

}