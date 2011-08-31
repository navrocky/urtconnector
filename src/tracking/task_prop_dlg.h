#ifndef TRACKING_TASK_PROP_DLG_H
#define	TRACKING_TASK_PROP_DLG_H

#include <memory>
#include <QDialog>
#include <QPointer>

#include "pointers.h"

class Ui_task_prop_dlg;

class QComboBox;
class QToolButton;
class QTreeWidgetItem;
class QLabel;

namespace tracking
{

class task_t;

class action_widget : public QWidget
{
    Q_OBJECT
public:
    action_widget(QTreeWidgetItem* item,
                  action_factory_p factory,
                  action_p action,
                  QWidget* parent);

    action_p current_action() const {return action_;}
    QTreeWidgetItem* item() const {return item_;}

signals:
    void action_created();
    void action_canceled();

public slots:
    void activate_select();
    
private slots:
    void combo_activated(int);
    void delete_action();

private:
    void update_contents();

    QTreeWidgetItem* item_;
    QWidget* new_action_panel_;
    QWidget* action_panel_holder_;
//    QToolButton* expand_btn_;
    QLabel* header_icon_;
    QLabel* header_label_;
    QToolButton* delete_btn_;
    QPointer<QWidget> action_panel_;
    QComboBox* action_select_combo_;
    QToolButton* cancel_btn_;
    action_factory_p factory_;
    action_p action_;
    bool block_action_combo_;
};

class task_prop_dlg : public QDialog
{
    Q_OBJECT
public:
    task_prop_dlg(task_t* t, 
                  const condition_factory_p& conds,
                  const action_factory_p& acts,
                  QWidget* parent);

public slots:
    virtual void accept();

protected:
    void closeEvent(QCloseEvent*);

private slots:
    void cond_combo_changed();
    void mode_combo_changed();
    void add_action();
    void action_canceled();
    void new_action_created();
    void delete_action();
    void update_actions();
    void delete_selected_action();
    void move_up();
    void move_down();

private:
    void move(int delta);
    void update_contents();
    void assign_action_to_item(QTreeWidgetItem*, const action_p&);
    action_p get_action_from_item(QTreeWidgetItem*);

    std::auto_ptr<Ui_task_prop_dlg> ui_;
    task_t* task_;
    condition_factory_p conds_;
    action_factory_p acts_;
    QPointer<QWidget> cond_widget_;
    QTreeWidgetItem* new_action_item_;
    bool updating_active_;
};

}

#endif	/* _TASK_PROP_DLG_H */
