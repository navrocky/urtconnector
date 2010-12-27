#include <QTimerEvent>

#include "qaccumulatingconnection.h"

QAccumulatingConnection::QAccumulatingConnection(int emitInterval, EmitMode mode, QObject* parent)
: QObject(parent)
, emitInterval_(emitInterval)
, timerId_(0)
, emitMode_(mode)
{
}

QAccumulatingConnection::QAccumulatingConnection(const QObject* sender, const char* signal,
        const QObject* receiver, const char* slot, int emitInterval,
        EmitMode mode, QObject* parent)
: QObject(parent)
, emitInterval_(emitInterval)
, timerId_(0)
, emitMode_(mode)
{
    QObject::connect(sender, signal, this, SLOT(emitSignal()));
    QObject::connect(this, SIGNAL(signal()), receiver, slot);
}

void QAccumulatingConnection::emitSignal()
{
    if (emitMode_ == Finally && timerId_)
    {
        killTimer(timerId_);
        timerId_ = 0;
    }
    if (!timerId_)
        timerId_ = startTimer(emitInterval_);
}

void QAccumulatingConnection::timerEvent(QTimerEvent* e)
{
    if (e->timerId() != timerId_)
        return;
    killTimer(timerId_);
    timerId_ = 0;
    emit signal();
}

void QAccumulatingConnection::emitNow()
{
    resetEmit();
    emit signal();
}

void QAccumulatingConnection::resetEmit()
{
    if (!timerId_)
        return;
    killTimer(timerId_);
    timerId_ = 0;
}
