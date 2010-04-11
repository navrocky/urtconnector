#include <QHBoxLayout>
#include <QLabel>
#include <QProgressBar>
#include <QToolButton>
#include <QIcon>
#include <QTimer>

#include "job_monitor.h"

job_monitor::job_monitor(job_queue* que, QWidget* parent)
: QWidget(parent)
, que_(que)
{
    update_timer_ = new QTimer(this);
    update_timer_->setInterval(100);
    connect(update_timer_, SIGNAL(timeout()), SLOT(update()));
    update_timer_->start();
    
    QHBoxLayout* lay = new QHBoxLayout(this);

    label_ = new QLabel("test test bla-bla bla andatra", this);
    lay->addWidget(label_);
    progress_ = new QProgressBar(this);
    progress_->setRange(0, 100);
    lay->addWidget(progress_);

    cancel_ = new QToolButton(this);
    lay->addWidget(cancel_);
    cancel_->setAutoRaise(true);
    cancel_->setIcon(QIcon(":/icons/icons/close.png"));
    cancel_->setToolTip(tr("Cancel active job"));

    open_ = new QToolButton(this);
    lay->addWidget(open_);
    open_->setAutoRaise(true);
    open_->setIcon(QIcon(":/icons/icons/go-up.png"));
    open_->setToolTip(tr("Open job queue"));
}

void job_monitor::update()
{
    job_p job = que_->get_current_job().lock();
    if (job)
    {
        QString capt = job->get_caption();
        if (label_->text() != capt)
            label_->setText(capt);
        progress_->setValue(job->get_progress());
        setVisible(true);
    } else
    {
        setVisible(false);
    }
}


