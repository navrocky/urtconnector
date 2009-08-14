#include <QPushButton>
#include <QAction>

#include "pushbuttonactionlink.h"

PushButtonActionLink::PushButtonActionLink(QPushButton * button, QAction * action)
    : button_(button),
      action_(action)
{
    connect(button, SIGNAL(clicked()), action, SLOT(trigger()));
    connect(action, SIGNAL(changed()), SLOT(assign()));
    assign();
}

void PushButtonActionLink::assign()
{
    button_->setEnabled(action_->isEnabled());
    button_->setVisible(action_->isVisible());
    button_->setIcon(action_->icon());
    button_->setText(action_->text());
    button_->setToolTip(action_->toolTip());
    button_->setWhatsThis(action_->whatsThis());
}