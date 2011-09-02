#ifndef TRACKING_TIMER_CONDITION_H
#define	TRACKING_TIMER_CONDITION_H

#include <QWidget>
#include <QPointer>

#include "../condition.h"

class QTimer;
class QSpinBox;

namespace tracking
{

class timer_condition_class : public condition_class
{
public:
    timer_condition_class(const context_p& ctx);
    condition_p create();
};


class timer_condition : public condition_t
{
    Q_OBJECT
public:
    timer_condition(const condition_class_p& c);

    int interval() const;
    void set_interval(int);

    void save(settings_t& s);
    void load(const settings_t& s);

    QWidget* create_options_widget(QWidget* parent);

    void skip_current();

protected:
    void do_start();
    void do_stop();

private:
    QTimer* timer_;
};

class timer_condition_widget : public QWidget
{
    Q_OBJECT
public:
    timer_condition_widget(QWidget* parent, timer_condition* cond);

private slots:
    void spin_changed(int);

private:
    QSpinBox* spin_;
    QPointer<timer_condition> cond_;
};

}

#endif
