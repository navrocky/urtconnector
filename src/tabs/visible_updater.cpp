#include "visible_updater.h"

#include <QWidget>
#include <QEvent>

visible_updater::visible_updater(QWidget* w, const char* slot, QObject* parent)
: QObject(parent)
, update_pended_(false)
, w_(w)
{
    w_->installEventFilter(this);
    connect(this, SIGNAL(update_needed()), w, slot);
}

bool visible_updater::eventFilter(QObject* o, QEvent* e)
{
    if (o == w_ && e->type() == QEvent::Show && update_pended_)
    {
        update_pended_ = false;
        emit update_needed();
    }
    return QObject::eventFilter(o, e);
}

void visible_updater::update_contents()
{
    if (w_->isVisible())
        emit update_needed();
    else
        update_pended_ = true;
}
