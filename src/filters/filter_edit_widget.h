#ifndef FILTER_EDIT_WIDGET_H
#define	FILTER_EDIT_WIDGET_H

#include <QMainWindow>
#include <QWidget>
#include <QDialog>

#include "pointers.h"

class QWidget;
class QTreeWidget;
class QTreeWidgetItem;
class QListWidget;
class QAction;
class QLabel;
class QComboBox;
class QCheckBox;
class QToolButton;
class QBoxLayout;
class QDialogButtonBox;

class composite_filter;

class select_filter_class_dialog : public QDialog
{
    Q_OBJECT
public:
    select_filter_class_dialog(filter_factory_p factory, QWidget* parent);

    filter_class_p selected() const {return selected_;}
private slots:
    void selection_changed();
private:
    void update_list();
    void update_actions();

    QListWidget* tree_;
    QDialogButtonBox* buttons_;
    filter_factory_p factory_;
    filter_class_p selected_;
};

class filter_item_widget : public QWidget
{
    Q_OBJECT
public:
    filter_item_widget(filter_p filter, QWidget* parent);
    void set_selected(bool val);
    void set_pin(bool val);

private slots:
    void enable_toggled();

private:
    void update_selected();
    void update_contents();

    bool selected_;
    filter_p filter_;
    QLabel* pin_label_;
    QLabel* label_;
    QCheckBox* enabled_check_;
    QToolButton* options_button_;
    QBoxLayout* options_lay_;
    QWidget* quick_opts_widget_;
};


class filter_edit_widget : public QMainWindow
{
    Q_OBJECT
public:
    filter_edit_widget(filter_list_p filters, QWidget* parent = NULL);

private slots:
    void add_new_filter();
    void delete_filter();
    void update_actions();
    void select_toolbar_filter();
    void item_changed();

private:
    void do_update(filter_p parent, QTreeWidget*, QTreeWidgetItem*);
    void update_item(QTreeWidgetItem*);
    void update_contents();
    composite_filter* composite_cast(filter_p);
    void update_items();

    filter_list_p filters_;
    QTreeWidget* tree_;
    QAction* add_new_filter_action_;
//    QAction* add_exist_filter_action_;
    QAction* delete_filter_action_;
    QAction* select_toolbar_filter_action_;
};

#endif	/* FILTER_EDIT_WIDGET_H */

