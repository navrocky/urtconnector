#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QIcon>
#include <QTimer>
#include <QPoint>
#include <QToolTip>

#include "ui_job_monitor.h"
#include "ui_job_item.h"

#include "job_monitor.h"

////////////////////////////////////////////////////////////////////////////////
// job_monitor

job_monitor::job_monitor(job_queue* que, QWidget* parent)
: QWidget(parent)
, ui_(new Ui_job_monitor)
, que_(que)
{
    ui_->setupUi(this);
    update_timer_ = new QTimer(this);
    update_timer_->setInterval(100);
    connect(update_timer_, SIGNAL(timeout()), SLOT(update_contents()));

    connect(ui_->cancel_button, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui_->show_button, SIGNAL(clicked()), SLOT(show()));
    connect(que_, SIGNAL(changed()), SLOT(job_que_changed()));
    popup_ = new job_que_popup(que_, this);

    setVisible(false);
}

void job_monitor::update_contents()
{
    job_p job = popup_->current_job();
    if (job)
    {
        QString capt = job->get_caption();
        if (ui_->label->text() != capt)
            ui_->label->setText(capt);
        ui_->progress->setValue(job->get_progress());
        setVisible(true);
        ui_->show_button->setEnabled(has_queued_jobs());
    } else
    {
        setVisible(false);
        update_timer_->stop();
    }
}

void job_monitor::cancel()
{
    job_p job = popup_->current_job();
    if (job)
        job->cancel();
}

void job_monitor::show()
{
    if (popup_->isVisible())
        popup_->hide();
    else
        popup_->show();
}

bool job_monitor::has_queued_jobs()
{
    const jobs_t& jobs = que_->get_jobs();
    return jobs.size() > 1;
}

void job_monitor::job_que_changed()
{
    update_timer_->start();
}

////////////////////////////////////////////////////////////////////////////////
// job_item

job_item::job_item(job_weak_p job, QWidget* parent)
: QWidget(parent)
, ui_(new Ui_job_item)
, job_(job)
{
    ui_->setupUi(this);
    connect(ui_->cancel_button, SIGNAL(clicked()), SLOT(cancel()));
}

void job_item::update_contents()
{
    job_p j = job_.lock();
    if (!j)
        return;

    ui_->label->setText(j->get_caption());
    ui_->progress->setValue(j->get_progress());
}

void job_item::cancel()
{
    job_p j = job_.lock();
    if (j)
        j->cancel();
}

////////////////////////////////////////////////////////////////////////////////
// job_que_popup

job_que_popup::job_que_popup(job_queue* que, QWidget* parent)
: QFrame(parent, Qt::ToolTip)
, que_(que)
{
    setPalette(QToolTip::palette());
    setFrameStyle(QFrame::Box);
    setLineWidth(1);

    connect(que_, SIGNAL(changed()), SLOT(update_contents()));

    update_timer_ = new QTimer(this);
    update_timer_->setInterval(100);
    connect(update_timer_, SIGNAL(timeout()), SLOT(update_contents()));

    hide_timer_ = new QTimer(this);
    hide_timer_->setSingleShot(true);
    hide_timer_->setInterval(2000);
    connect(hide_timer_, SIGNAL(timeout()), SLOT(update_contents()));

    lay_ = new QVBoxLayout(this);
}

job_p job_que_popup::current_job()
{
    const jobs_t& jobs = que_->get_jobs();
    if (jobs.isEmpty())
        return job_p();
    else
        return jobs.first();
}

void job_que_popup::correct_position()
{
    QPoint pt = parentWidget()->mapToGlobal(QPoint(parentWidget()->width(), 0));
    QSize sz = minimumSizeHint();
    int height = sz.height();
    int width = sz.width();
    setGeometry(pt.x() - width, pt.y() - height, width, height);
}

void job_que_popup::setVisible(bool visible)
{
    if (visible)
    {
        update_contents();
        update_timer_->start();
    } else
    {
        update_timer_->stop();
    }

    QWidget::setVisible(visible);
}

job_item* job_que_popup::find_item(job_p j)
{
    foreach (job_item* item, items_)
    {
        if (item->job() == j)
            return item;
    }
    return 0;
}

class item_expired_and_delete
{
public:
    item_expired_and_delete(job_p current_job_)
    : current_job(current_job_)
    {
    }

    bool operator()(const job_item* item) const
    {
        const job_p& job = item->job();
        bool res = !job || job->get_state() == job_t::js_not_started || job == current_job;
        if (res)
            delete item;
        return res;
    }

    job_p current_job;
};

void job_que_popup::update_contents()
{
    bool job_added = false;
    // adding created
    foreach (const job_p& job, que_->get_jobs())
    {
        if (job == current_job())
            continue;
        job_item* item = find_item(job);
        if (!item)
        {
            item = new job_item(job, this);
            item->show();
            items_.append(item);
            lay_->addWidget(item);//insertWidget(0, item);
            job_added = true;
        }
        item->update_contents();
    }

    // remove expired
    items_.erase(std::remove_if(items_.begin(), items_.end(), item_expired_and_delete(current_job())), items_.end());

    if (items_.isEmpty())
        hide();
    else
    {
        correct_position();
        // commented because it
//        if (job_added)
//            show_temporarily();
    }
}

void job_que_popup::show()
{
    update_contents();
    hide_timer_->stop();
    if (!isVisible())
        QFrame::show();
}

void job_que_popup::hide()
{
    hide_timer_->stop();
    if (isVisible())
        QFrame::hide();
}

void job_que_popup::show_temporarily()
{
    hide_timer_->start();
    if (!isVisible())
        QFrame::show();
}

void job_que_popup::hide_by_timer()
{
    hide_timer_->stop();
    if (isVisible())
        QFrame::hide();
}

