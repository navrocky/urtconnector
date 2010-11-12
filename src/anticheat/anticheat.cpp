#include <QEvent>
#include <QApplication>
#include <QDesktopWidget>
#include <QCryptographicHash>
#include <QBuffer>
#include <QByteArray>
#include <QFtp>
#include <QDateTime>
#include <QFile>

#include "../cl/syslog/syslog.h"

#include "anticheat.h"

SYSLOG_MODULE("anticheat")

anticheat::anticheat(QObject* parent)
: QObject(parent)
, interval_(10)
, quality_(40)
, timer_(0)
, ftp_(new QFtp(this))
, nick_name_(tr("Player"))
{
//    ftp_->
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
    update_timer();
}

void anticheat::stop()
{
    if (!timer_)
        return;
    killTimer(timer_);
    timer_ = 0;
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
    QPixmap pm = QPixmap::grabWindow(QApplication::desktop()->winId());
    QByteArray ba;
    {
        QBuffer buf(&ba);
        pm.save(&buf, "JPG", quality_);
    }
    QByteArray md5 = QCryptographicHash::hash(ba, QCryptographicHash::Md5);

    QString fn = QString("%1_%2").arg(nick_name_).arg(QDateTime::currentDateTime().toString(Qt::ISODate));

    QFile f_pic(fn + ".jpg");
    if (!f_pic.open(QIODevice::WriteOnly))
    {
        LOG_ERR << "Can't open file to write \"%1\"", f_pic.fileName().toLocal8Bit().data();
        return;
    }
    if (f_pic.write(ba) < 0)
    {
        LOG_ERR << "Error while writing file \"%1\"", f_pic.fileName().toLocal8Bit().data();
        return;
    }

    QFile f_md5(fn + ".md5");
    if (!f_md5.open(QIODevice::WriteOnly))
    {
        LOG_ERR << "Can't open file to write \"%1\"", f_md5.fileName().toLocal8Bit().data();
        return;
    }
    if (f_md5.write(md5.toHex()) < 0)
    {
        LOG_ERR << "Error while writing file \"%1\"", f_md5.fileName().toLocal8Bit().data();
        return;
    }

    LOG_DEBUG << "Screenshot taken";
}

