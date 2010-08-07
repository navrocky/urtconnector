#include <QTreeWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCheckBox>
#include <QLabel>
#include <QToolButton>
#include <QComboBox>
#include <QHeaderView>

#include "filter.h"
#include "filter_list.h"
#include "composite_filter.h"

#include "filter_edit_widget.h"

Q_DECLARE_METATYPE(filter_p)

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

    label_ = new QLabel(this);
//    label_->setText("<b>Filter name</b><br><small>Filter description here.</small>");
//    lb->setForegroundRole(QPalette::HighlightedText);

//    QFont fn = lb->font();
//    fn.setBold(true);
//    lb->setFont(fn);
    label_->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    lay->addWidget(enabled_check_);
    lay->addWidget(label_);

    options_lay_ = new QHBoxLayout(this);
    options_lay_->setContentsMargins(0, 0, 0, 0);
    lay->addLayout(options_lay_);

//    QComboBox* combo = new QComboBox(this);
//    lay->addWidget(combo);
//    combo->addItem("AND");
//    combo->addItem("OR");
//    combo->addItem("XOR %D");
//    combo->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    options_button_ = new QToolButton(this);
    options_button_->setIcon(QIcon(":/icons/icons/configure.png"));
    options_button_->setToolTip(tr("Configure filter"));
    lay->addWidget(options_button_);

//    btn = new QToolButton(this);
//    btn->setIcon(QIcon(":/icons/icons/info.png"));
//    btn->setAutoRaise(true);
//    lay->addWidget(btn);
//
//    btn = new QToolButton(this);
//    btn->setIcon(QIcon(":/icons/icons/delete.png"));
////    btn->setAutoRaise(true);
//    lay->addWidget(btn);

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
    QString name = filter_->name();
    if (name.isEmpty())
        name = fc->caption();

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
    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    tree_ = new QTreeWidget(this);
    lay->addWidget(tree_);
    tree_->header()->setVisible(false);
    tree_->setAlternatingRowColors(true);

    resize(400, 250);
    update_contents();
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

}

void filter_edit_widget::update_contents()
{
    tree_->clear();
    do_update(filters_->root_filter(), tree_, 0);
}
