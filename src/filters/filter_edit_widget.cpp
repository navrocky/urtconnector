#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QHeaderView>
#include <QAction>
#include <QListWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <qmessagebox.h>
#include <qapplication.h>

#include "filter.h"
#include "filter_list.h"
#include "composite_filter.h"

#include "filter_edit_widget.h"
#include "filter_factory.h"
#include "server_list_widget.h"

Q_DECLARE_METATYPE(filter_p)
Q_DECLARE_METATYPE(filter_class_p)

////////////////////////////////////////////////////////////////////////////////
// select_filter_class_dialog

select_filter_class_dialog::select_filter_class_dialog(filter_factory_p factory)
: QDialog()
, factory_(factory)
{
    setWindowTitle(tr("Select filter type"));
    setWindowIcon(QIcon(":/icons/icons/zoom.png"));

    QBoxLayout* lay = new QVBoxLayout(this);
    tree_ = new QListWidget(this);
    connect(tree_, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
            SLOT(selection_changed()));
    lay->addWidget(tree_);
    
    buttons_ = new QDialogButtonBox(this);
    buttons_->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    lay->addWidget(buttons_);
    connect(buttons_, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons_, SIGNAL(rejected()), SLOT(reject()));
    connect(tree_, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            buttons_->button(QDialogButtonBox::Ok), SLOT(click()));

    update_list();
    update_actions();
}

void select_filter_class_dialog::selection_changed()
{
    update_actions();
}

void select_filter_class_dialog::update_actions()
{
    QListWidgetItem* item = tree_->currentItem();
    if (item)
        selected_ = item->data(Qt::UserRole).value<filter_class_p>();
    else
        selected_ = filter_class_p();

    buttons_->button(QDialogButtonBox::Ok)->setEnabled(selected_);
}

void select_filter_class_dialog::update_list()
{
    tree_->clear();
    foreach (filter_class_p fc, factory_->filter_classes())
    {
        QListWidgetItem* it = new QListWidgetItem(fc->icon(), fc->caption(), tree_);
        it->setToolTip(fc->description());
        it->setData(Qt::UserRole, QVariant::fromValue(fc));
    }
}

////////////////////////////////////////////////////////////////////////////////
// filter_item_widget

filter_item_widget::filter_item_widget(filter_p filter, QWidget* parent)
: QWidget(parent)
, selected_(false)
, filter_(filter)
, quick_opts_widget_(NULL)
{
    QBoxLayout* lay = new QHBoxLayout(this);
    enabled_check_ = new QCheckBox(this);
    enabled_check_->setToolTip(tr("Enable filter"));
    enabled_check_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    connect(enabled_check_, SIGNAL(stateChanged(int)), SLOT(enable_toggled()));

    int is = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    pin_label_ = new QLabel(this);
    pin_label_->setPixmap(QIcon(":/icons/icons/pin.png").pixmap(is));
    pin_label_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    pin_label_->setVisible(false);

    label_ = new QLabel(this);
    label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(enabled_check_);
    lay->addWidget(pin_label_);
    lay->addWidget(label_);

    options_lay_ = new QHBoxLayout;
    options_lay_->setContentsMargins(0, 0, 0, 0);
    lay->addLayout(options_lay_);

    options_button_ = new QToolButton(this);
    options_button_->setIcon(QIcon(":/icons/icons/configure.png"));
    options_button_->setToolTip(tr("Configure filter"));
    lay->addWidget(options_button_);

    update_contents();
}

void filter_item_widget::enable_toggled()
{
    filter_->set_enabled(enabled_check_->isChecked());
}

void filter_item_widget::set_selected(bool val)
{
    if (selected_ == val)
        return;
    selected_ = val;
    update_selected();
}

void filter_item_widget::set_pin(bool val)
{
    if (pin_label_->isVisible() == val)
        return;
    pin_label_->setVisible(val);
}

void filter_item_widget::update_selected()
{
    QList<QLabel*> labels = findChildren<QLabel*>();
    foreach (QLabel* lb, labels)
    {
        if (selected_)
            lb->setForegroundRole(QPalette::HighlightedText);
        else
            lb->setForegroundRole(QPalette::Text);
    }
}

void filter_item_widget::update_contents()
{
    filter_class_p fc = filter_->get_class();
    QString name = fc->caption();
    label_->setText(name);
    label_->setToolTip(fc->description());
    options_button_->setVisible(fc->has_additional_options());
    enabled_check_->setChecked(filter_->enabled());

    if (!quick_opts_widget_)
    {
        quick_opts_widget_ = fc->create_quick_opts_widget(filter_);
        if (quick_opts_widget_)
        {
            quick_opts_widget_->setParent(this);
            options_lay_->addWidget(quick_opts_widget_);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////
// filter_edit_widget

filter_edit_widget::filter_edit_widget(filter_list_p filters, QWidget* parent)
: QWidget(parent, Qt::Tool)
, filters_(filters)
{
    setWindowTitle(tr("Filter options"));
    setWindowIcon(QIcon(":/icons/icons/view-filter.png"));

    add_new_filter_action_ = new QAction(QIcon(":/icons/icons/add.png"),
                                         tr("Add new child filter"), this);
    delete_filter_action_ = new QAction(QIcon(":/icons/icons/remove.png"),
                                         tr("Delete filter"), this);
    select_toolbar_filter_action_ = new QAction(QIcon(":/icons/icons/pin.png"),
                                         tr("Select filter for toolbar"), this);

    connect(add_new_filter_action_, SIGNAL(triggered()), SLOT(add_new_filter()));
    connect(select_toolbar_filter_action_, SIGNAL(triggered()), SLOT(select_toolbar_filter()));
    connect(delete_filter_action_, SIGNAL(triggered()), SLOT(delete_filter()));

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    tree_ = new QTreeWidget(this);
    lay->addWidget(tree_);
    tree_->header()->setVisible(false);
    tree_->setAlternatingRowColors(true);

    tree_->addAction(add_new_filter_action_);
    tree_->addAction(select_toolbar_filter_action_);
    QAction* sep = new QAction(this);
    sep->setSeparator(true);
    tree_->addAction(sep);
    tree_->addAction(delete_filter_action_);
    tree_->setContextMenuPolicy(Qt::ActionsContextMenu);

    connect(tree_, SIGNAL(currentItemChanged(QTreeWidgetItem*, QTreeWidgetItem*)),
            SLOT(item_changed()));

    resize(400, 250);
    update_contents();
}

void filter_edit_widget::item_changed()
{
    update_actions();
    update_items();
}

composite_filter* filter_edit_widget::composite_cast(filter_p f)
{
    return dynamic_cast<composite_filter*>(f.get());
}

void filter_edit_widget::do_update(filter_p filter, QTreeWidget* tw, QTreeWidgetItem* par_item)
{
    QTreeWidgetItem* item;
    if (par_item)
        item = new QTreeWidgetItem(par_item);
    else
        item = new QTreeWidgetItem(tw);

    item->setData(0, Qt::UserRole, QVariant::fromValue(filter));
    update_item(item);

    composite_filter* cf = composite_cast(filter);
    if (cf)
        foreach (filter_p f, cf->filters())
            do_update(f, tw, item);
}

void filter_edit_widget::update_item(QTreeWidgetItem* item)
{
    filter_p filter = item->data(0, Qt::UserRole).value<filter_p>();

    filter_item_widget* w = qobject_cast<filter_item_widget*>(tree_->itemWidget(item, 0));
    if (!w)
    {
        w = new filter_item_widget(filter, tree_);
        tree_->setItemWidget(item, 0, w);
    }
    w->set_pin(filter == filters_->toolbar_filter().lock());
    w->set_selected(tree_->currentItem() == item);
}

void filter_edit_widget::update_contents()
{
    tree_->clear();
    do_update(filters_->root_filter(), tree_, 0);
    tree_->expandAll();
}

void filter_edit_widget::update_items()
{
    QList<QTreeWidgetItem*> l = tree_->findItems("", Qt::MatchRecursive | Qt::MatchContains);
    foreach (QTreeWidgetItem* i, l)
    {
        update_item(i);
    }
}

void filter_edit_widget::update_actions()
{
    QTreeWidgetItem* item = tree_->currentItem();
    QTreeWidgetItem* parent_item = item ? item->parent() : NULL;
    filter_p f = item->data(0, Qt::UserRole).value<filter_p>();

    add_new_filter_action_->setEnabled(composite_cast(f));
    delete_filter_action_->setEnabled(parent_item);

    select_toolbar_filter_action_->setEnabled(f != filters_->toolbar_filter().lock());
}

void filter_edit_widget::add_new_filter()
{
    select_filter_class_dialog d(filters_->factory());
    if (d.exec() != QDialog::Accepted)
        return;
    filter_class_p fc = d.selected();

    QTreeWidgetItem* parent_item = tree_->currentItem();
    filter_p parent_f = parent_item->data(0, Qt::UserRole).value<filter_p>();
    composite_filter* cf = qobject_cast<composite_filter*>(parent_f.get());

    // create filter
    filter_p f = fc->create_filter();

    // assign auto-generated name
    f->set_name(filters_->correct_name(fc->caption()));

    cf->add_filter(f);
    QTreeWidgetItem* item = new QTreeWidgetItem(parent_item);
    item->setData(0, Qt::UserRole, QVariant::fromValue(f));
    update_item(item);
    tree_->setCurrentItem(item);
}

void filter_edit_widget::delete_filter()
{
    if (QMessageBox::question(this, tr("Delete filter"), tr("Continue with delete selected filter?"),
                          QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)
            != QMessageBox::Yes)
        return;
    QTreeWidgetItem* item = tree_->currentItem();
    QTreeWidgetItem* parent_item = item->parent();
    if (!parent_item)
        return;
    filter_p f = item->data(0, Qt::UserRole).value<filter_p>();
    filter_p parent_f = parent_item->data(0, Qt::UserRole).value<filter_p>();
    composite_filter* cf = qobject_cast<composite_filter*>(parent_f.get());
    cf->remove_filter(f);
    delete item;
}

void filter_edit_widget::select_toolbar_filter()
{
    QTreeWidgetItem* item = tree_->currentItem();
    if (!item)
        return;
    filter_p f = item->data(0, Qt::UserRole).value<filter_p>();
    filters_->set_toolbar_filter(f);
    update_items();
}