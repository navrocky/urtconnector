#ifndef _JOB_H
#define	_JOB_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include <QString>

class job_t : public QObject
{
    Q_OBJECT
public:
    enum state_t
    {
        js_not_started,
        js_executing,
        js_finished,
        js_canceled
    };

    job_t();

    // caption of job
    virtual QString get_caption() = 0;

    // start job
    virtual void start() = 0;

    // cancel job
    virtual void cancel() = 0;

    // progress in percents
    virtual int get_progress() = 0;

    // current job state
    state_t get_state() const {return state_;}

    // job is stopped
    bool is_stopped();

    static bool state_is_stopped(state_t state);

signals:
    void state_changed(job_t::state_t state);

protected:
    void set_state(state_t state);

private:
    state_t state_;

};

typedef boost::shared_ptr<job_t> job_p;
typedef boost::weak_ptr<job_t> job_weak_p;

#endif	/* _JOB_H */

