#ifndef _JOB_MONITOR_H
#define	_JOB_MONITOR_H

#include <memory>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <QWidget>
#include <QFrame>

#include "job_queue.h"

class QTimer;
class QLayout;

class Ui_job_monitor;
class Ui_job_item;

class job_item : public QWidget
{
    Q_OBJECT
public:
    job_item(job_weak_p job, QWidget* parent = 0);
    virtual ~job_item();

    const job_weak_p& job() const {return job_;}

private slots:
    void cancel();

private:
    void update();
    std::auto_ptr<Ui_job_item> ui_;
    job_weak_p job_;
};

typedef boost::shared_ptr<job_item> job_item_p;

class job_que_popup : public QFrame
{
    Q_OBJECT
public:
    job_que_popup(job_queue* que, QWidget* parent = 0);

    void setVisible(bool);

public slots:
    void correct_position();
    void update();

private:
    job_item* find_item(job_p j);

    typedef std::vector<job_item_p> items_t;
    items_t items_;
    job_queue* que_;
    QLayout* lay_;
    QTimer* update_timer_;
};

class job_monitor : public QWidget
{
    Q_OBJECT
public:
    job_monitor(job_queue* que, QWidget* parent = 0);
    virtual ~job_monitor();

private slots:
    void update();
    void cancel();
    void show();
    void job_added(job_weak_p);
    void show_popup();
    void hide_popup();
    void show_popup_temp();


private:
    bool has_queued_jobs();

    std::auto_ptr<Ui_job_monitor> ui_;
    job_queue* que_;
    QTimer* update_timer_;
    QTimer* popup_timer_;
    job_weak_p cur_job_;
    job_que_popup* popup_;
};


#endif	/* _JOB_MONITOR_H */

