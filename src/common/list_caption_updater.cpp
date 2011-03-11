#include "list_caption_updater.h"

#include <QWidget>

list_caption_updater::list_caption_updater(QWidget* widget, const QString& caption)
: widget_(widget)
, caption_(caption)
, visible_count_(0)
, total_count_(0)
{
    update_caption();
}

void list_caption_updater::set_visible_count ( int val )
{
    if ( visible_count_ == val)
        return;
    visible_count_ = val;
    update_caption();
}

void list_caption_updater::set_total_count ( int val )
{
    if ( total_count_ == val)
        return;
    total_count_ = val;
    update_caption();
}

void list_caption_updater::update_caption()
{
    QString cnt;
    if ( total_count_ > 0)
    {
        if ( visible_count_ == total_count_)
            cnt = QString(" (%1)").arg( visible_count_);
        else
            cnt = QString(" (%1/%2)").arg( visible_count_).arg( total_count_);
    }
    widget_->setWindowTitle(QString("%1%2").arg(caption_).arg(cnt));
}


