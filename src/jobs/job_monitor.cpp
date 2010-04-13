#include <QVBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QIcon>
#include <QTimer>
#include <QPoint>

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
    connect(update_timer_, SIGNAL(timeout()), SLOT(update()));

    popup_timer_ = new QTimer(this);
    popup_timer_->setSingleShot(true);
    popup_timer_->setInterval(2000);
    connect(popup_timer_, SIGNAL(timeout()), SLOT(hide_popup()));

    connect(ui_->cancel_button, SIGNAL(clicked()), SLOT(cancel()));
    connect(ui_->show_button, SIGNAL(clicked()), SLOT(show()));
    connect(que_, SIGNAL(job_added(job_weak_p)), SLOT(job_added(job_weak_p)));
    popup_ = new job_que_popup(que_, this);

    setVisible(false);
}

void job_monitor::update()
{
    job_p job = que_->get_current_job().lock();
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

    if (!has_queued_jobs() && popup_->isVisible())
    {
        hide_popup();
    }
}

void job_monitor::cancel()
{
    job_p job = que_->get_current_job().lock();
    if (job)
        job->cancel();
}

void job_monitor::show()
{
    if (popup_->isVisible())
        hide_popup();
    else
        show_popup();
}

bool job_monitor::has_queued_jobs()
{
    const jobs_t& jobs = que_->get_jobs();
    for (jobs_t::const_iterator it(jobs.begin()); it != jobs.end(); it++)
    {
        job_p job = *it;
        job_t::state_t state = job->get_state();
        if (state == job_t::js_not_started)
            return true;
    }
    return false;
}

void job_monitor::job_added(job_weak_p)
{
    update_timer_->start();
    if (has_queued_jobs())
        show_popup_temp();
}

void job_monitor::show_popup()
{
    popup_->correct_position();
    popup_->show();
    popup_timer_->stop();
}

void job_monitor::show_popup_temp()
{
    popup_->correct_position();
    popup_->show();
    popup_timer_->start();
}

void job_monitor::hide_popup()
{
    popup_timer_->stop();
    popup_->hide();
}

////////////////////////////////////////////////////////////////////////////////
// job_item

job_item::job_item(job_weak_p job, QWidget* parent)
: QWidget(parent)
, ui_(new Ui_job_item)
, job_(job)
{
    ui_->setupUi(this);
    update();
    connect(ui_->cancel_button, SIGNAL(clicked()), SLOT(cancel()));
}

void job_item::update()
{
    job_p j = job_.lock();
    if (j)
    {
        ui_->label->setText(j->get_caption());
    } else
    {
        ui_->label->setText("---");
    }
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
: QWidget(parent, Qt::ToolTip)
, que_(que)
{
    update_timer_ = new QTimer(this);
    update_timer_->setInterval(100);
    connect(update_timer_, SIGNAL(timeout()), SLOT(update()));

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay_ = lay;
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
        update();
        update_timer_->start();
    } else
    {
        update_timer_->stop();
    }
    
    QWidget::setVisible(visible);
}

job_item* job_que_popup::find_item(job_p j)
{
    for (items_t::iterator it(items_.begin()); it != items_.end(); it++)
    {
        if ((*it)->job().lock() == j)
            return (*it).get();
    }
    return 0;
}

class item_expired
{
public:
    bool operator()(const job_item_p& item) const
    {
        return item->job().expired() || item->job().lock()->get_state() != job_t::js_not_started;
    }
};


void job_que_popup::update()
{
    const jobs_t& jobs = que_->get_jobs();
//    lay_->setEnabled(false);

    // adding created
    for (jobs_t::const_iterator it(jobs.begin()); it != jobs.end(); it++)
    {
        job_p job = *it;
        if (find_item(job) != NULL) continue;

        job_item_p item(new job_item(job, this));
        items_.push_back(item);
        lay_->addWidget(item.get());
    }

    // remove expired
    items_.erase(std::remove_if(items_.begin(), items_.end(), item_expired()), items_.end());
    correct_position();
    
//    lay_->setEnabled(true);
}

