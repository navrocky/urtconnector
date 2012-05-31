#ifndef MINIMUM_EMPTY_SLOTS_FILTER_H
#define	MINIMUM_EMPTY_SLOTS_FILTER_H

#include <QPointer>
#include <QWidget>

#include <common/server_info.h>
#include "pointers.h"
#include "filter.h"

class QSpinBox;

class min_empty_slots_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    min_empty_slots_filter_quick_opt_widget(filter_p f, QWidget* parent);

private slots:
    void filter_changed();
    void spin_changed();

private:
    filter_p filter_;
    QSpinBox* spin_;
    bool block_filter_change_;
    bool block_ctl_change_;
};

class minimum_empty_slots_filter_class : public filter_class
{
public:
    minimum_empty_slots_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};

class minimum_empty_slots_filter : public filter
{
    Q_OBJECT
public:
    minimum_empty_slots_filter(filter_class_p fc);

    int minimum_empty_slots() const {return minimum_;}
    void set_minimum_empty_slots(int val);

    virtual bool filter_server(const server_info& si, filter_context& ctx);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    int minimum_;
};

#endif

