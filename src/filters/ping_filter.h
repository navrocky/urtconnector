
#ifndef PING_FILTER_H
#define PING_FILTER_H

#include <QWidget>

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>


class ping_filter;

class ping_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    ping_filter_quick_opt_widget(filter_p f, QWidget* parent);

private:
    ping_filter&  filter_;
};

class ping_filter_class : public filter_class
{
public:
    ping_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};

class ping_filter : public filter
{
    Q_OBJECT
public:
    
    enum Type{ more, less, between };
    
    ping_filter(filter_class_p fc);

    virtual bool filter_server(const server_info& si, filter_context& ctx);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

    int min() const { return borders_.first; }
    int max() const { return borders_.second; }
    Type type() const { return type_; }
    
    void set_min(int min){ borders_.first = min; emit changed_signal(); }
    void set_max(int max){ borders_.second = max; emit changed_signal(); }
    void set_type( Type t){ type_ = t; emit changed_signal(); }
   
private:
    QPair<int, int> borders_;
    Type            type_;
};


#endif

