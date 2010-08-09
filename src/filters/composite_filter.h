#ifndef COMPOSITE_FILTER_H
#define	COMPOSITE_FILTER_H

#include <QPointer>
#include <QComboBox>

#include "pointers.h"
#include "filter.h"

class composite_filter_quick_opt_widget : public QComboBox
{
    Q_OBJECT
public:
    composite_filter_quick_opt_widget(filter_p f);
private:
    filter_p filter_;
};

class composite_filter_class : public filter_class
{
public:
    composite_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f);
    virtual filter_p create_filter();

    static const char* get_id();
};

class composite_filter : public filter
{
    Q_OBJECT
public:
    enum operation_t
    {
        op_and,
        op_or
    };

    composite_filter(filter_class_p fc);
    ~composite_filter();

    operation_t operation() const {return operation_;}
    void set_operation(operation_t op);

    void add_filter(filter_p f);
    void remove_filter(filter_p f);
    const filters_t& filters() const {return filters_;}

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private slots:
    void child_filter_changed();

private:
    operation_t operation_;
    filters_t filters_;
    QPointer<QComboBox> combo_;
};

#endif	/* COMPOSITE_FILTER_H */

