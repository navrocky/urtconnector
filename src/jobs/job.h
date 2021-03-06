#ifndef _JOB_H
#define	_JOB_H

#include <QObject>
#include <QString>
#include <QPointer>

class QEventLoop;

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
    QString caption() const {return caption_;}

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

    bool is_canceled();

    static bool state_is_stopped(state_t state);

    /*! Starts local event loop and waits to job finish */
    void wait_for_finish();


    // deprectaed
    void start_and_wait_for_finish();

signals:
    void state_changed(job_t::state_t state);
    void caption_changed();

protected:
    void set_state(state_t state);
    void set_caption(const QString&);

private:
    state_t state_;
    QPointer<QEventLoop> event_loop_;
    QString caption_;
};

#endif	/* _JOB_H */
