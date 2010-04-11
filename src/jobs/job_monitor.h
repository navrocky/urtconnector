#ifndef _JOB_MONITOR_H
#define	_JOB_MONITOR_H

#include <QWidget>

#include "job_queue.h"

class QLabel;
class QProgressBar;
class QToolButton;
class QTimer;

class job_monitor : public QWidget
{
    Q_OBJECT
public:
    job_monitor(job_queue* que, QWidget* parent = 0);

private slots:
    void update();

private:
    job_queue* que_;
    QLabel* label_;
    QProgressBar* progress_;
    QToolButton* cancel_;
    QToolButton* open_;
    QTimer* update_timer_;
    job_weak_p cur_job_;
};

#endif	/* _JOB_MONITOR_H */

