#ifndef _JOB_MONITOR_H
#define	_JOB_MONITOR_H

#include <memory>

#include <QList>
#include <QWidget>
#include <QFrame>

#include "job_queue.h"
#include "pointers.h"

class QTimer;
class QBoxLayout;

class Ui_job_monitor;
class Ui_job_item;

class job_item : public QWidget
{
    Q_OBJECT
public:
    job_item(job_weak_p job, QWidget* parent = 0);

    job_p job() const {return job_.lock();}

    void update_contents();

private slots:
    void cancel();

private:
    std::auto_ptr<Ui_job_item> ui_;
    job_weak_p job_;
};

class job_que_popup : public QFrame
{
    Q_OBJECT
public:
    job_que_popup(job_queue* que, QWidget* parent = 0);

    void setVisible(bool);

    job_p current_job();

    void show();
    void hide();
    void show_temporarily();

public slots:
    void correct_position();
    void update_contents();
    void hide_by_timer();

private:

    job_item* find_item(job_p j);

    typedef QList<job_item*> items_t;
    items_t items_;
    job_queue* que_;
    QBoxLayout* lay_;
    QTimer* update_timer_;
    QTimer* hide_timer_;
};

class job_monitor : public QWidget
{
    Q_OBJECT
public:
    job_monitor(job_queue* que, QWidget* parent);

private slots:
    void update_contents();
    void cancel();
    void show();
    void job_que_changed();

private:
    bool has_queued_jobs();

    std::auto_ptr<Ui_job_monitor> ui_;
    job_queue* que_;
    QTimer* update_timer_;
    job_weak_p cur_job_;
    job_que_popup* popup_;
};


#endif	/* _JOB_MONITOR_H */

