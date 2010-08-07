#include "history_widget.h"
#include "ui_history_widget.h"
#include <boost/shared_ptr.hpp>
#include "history.h"

#include <QList>

history_widget::history_widget(app_options_p opts, QWidget *parent, history_p list) :
    QWidget(parent),
    ui_(new Ui::history_widget),
    history_(list)
{
    ui_->setupUi(this);
    connect(ui_->clearFilterButton, SIGNAL(clicked()), SLOT(filter_clear()));
}

int history_widget::num_rows() const
{
    return ui_->treeWidget->topLevelItemCount();
}

history_widget::~history_widget()
{
    delete ui_;
}

void history_widget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui_->retranslateUi(this);
        break;
    default:
        break;
    }
}

void history_widget::filter_clear()
{
    ui_->filterEdit->clear();
}

QTreeWidget* history_widget::tree() const
{
    return ui_->treeWidget;
}

void history_widget::update_history()
{
   ui_->treeWidget->clear();
   int i;
   for (i = 0; i < history_->length(); i++)
   {
       addItem(history_->at(i));
   }
}

void history_widget::addItem(history_item_p item)
{
    //boost::shared_ptr<QTreeWidgetItem> item_ptr(new QTreeWidgetItem(ui_->treeWidget));
    QTreeWidgetItem* item_ptr = new QTreeWidgetItem();
    item_ptr->setText(0, item->date_time().toString());
    item_ptr->setText(1, item->server_name());
    item_ptr->setText(2, item->address());
    item_ptr->setText(3, item->password());
    item_ptr->setText(4, item->player_name());
    ui_->treeWidget->insertTopLevelItem(0, item_ptr);
}
