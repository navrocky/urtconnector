#include <QEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include <QBuffer>
#include <QByteArray>
#include <QDateTime>

#include <cl/syslog/syslog.h>

#include "sshot_output.h"
#include "anticheat.h"

SYSLOG_MODULE(anticheat)

namespace anticheat
{

anticheat::anticheat(QObject* parent)
: QObject(parent)
, interval_(10)
, quality_(40)
, timer_(0)
, nick_name_(tr("Player"))
{
}

anticheat::~anticheat()
{
    stop();
}

void anticheat::set_interval(int val)
{
    if (interval_ == val)
        return;
    interval_ = val;
    if (timer_)
        update_timer();
}

void anticheat::set_quality(int val)
{
    if (val < 0)
        val = 0;
    if (val > 100)
        val = 100;
    quality_ = val;
}

void anticheat::set_nick_name(const QString& val)
{
    nick_name_ = val;
}

void anticheat::update_timer()
{
    if (timer_)
        killTimer(timer_);
    timer_ = startTimer(interval_);
}

void anticheat::start()
{
    if (timer_)
        return;

    LOG_DEBUG << "Anticheat started";

    update_timer();

    foreach (sshot_output* out, outputs_)
    {
        if (out)
            out->start();
    }
}

void anticheat::stop()
{
    if (!timer_)
        return;
    LOG_DEBUG << "Anticheat stopped";

    killTimer(timer_);
    timer_ = 0;

    foreach (sshot_output* out, outputs_)
    {
        if (out)
            out->stop();
    }
}

bool anticheat::event(QEvent* e)
{
    if (e->type() == QEvent::Timer)
        if (static_cast<QTimerEvent*>(e)->timerId() == timer_)
            screen_shot();

    return QObject::event(e);
}

void anticheat::screen_shot()
{
    bool can_send = false;
    foreach (sshot_output* out, outputs_)
    {
        if (out && out->can_send_now())
        {
            can_send = true;
            break;
        }
    }
    if (!can_send)
    {
        LOG_DEBUG << "There are pending output commands, screenshot send skipped";
        return;
    }

    QPixmap pm = QPixmap::grabWindow(QApplication::desktop()->winId());
    QByteArray ba;
    {
        QBuffer buf(&ba);
        pm.save(&buf, "JPG", quality_);
    }
    QByteArray md5 = QCryptographicHash::hash(ba, QCryptographicHash::Md5);

    QString fn = QString("%1_%2").arg(nick_name_).arg(QDateTime::currentDateTime().toString(Qt::ISODate));

    foreach (sshot_output* out, outputs_)
    {
        if (!out || !out->can_send_now())
            continue;
        out->send_file(fn + ".jpg", ba);
        out->send_file(fn + ".md5", md5);
    }

    LOG_DEBUG << "Screenshot taken";
}

void anticheat::add_output(sshot_output* output)
{
    outputs_.append(output);
}

bool anticheat::is_started()
{
    return timer_ != 0;
}

}
