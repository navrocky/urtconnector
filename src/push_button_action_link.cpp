#include <QPushButton>
#include <QAction>

#include "push_button_action_link.h"

push_button_action_link::push_button_action_link(QObject* parent, QPushButton * button, QAction * action)
    : QObject(parent),
      button_(button),
      action_(action)
{
    connect(button, SIGNAL(clicked()), action, SLOT(trigger()));
    connect(action, SIGNAL(changed()), SLOT(assign()));
    assign();
}

void push_button_action_link::assign()
{
    button_->setEnabled(action_->isEnabled());
    button_->setVisible(action_->isVisible());
    button_->setIcon(action_->icon());
    button_->setText(action_->text());
    button_->setToolTip(action_->toolTip());
    button_->setWhatsThis(action_->whatsThis());
}