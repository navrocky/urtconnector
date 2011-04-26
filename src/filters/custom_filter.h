
#ifndef CUSTOM_FILTER_H
#define CUSTOM_FILTER_H


#include <QDialog>

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>

class custom_filter;

///Form to edit custom filter contents
class custom_filter_editor: public QDialog{
    Q_OBJECT
public:
    custom_filter_editor( custom_filter* f, QWidget* parent );
};


class custom_filter_class : public filter_class
{
public:
    custom_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};


///Allow to combine multiple filters into single one
class custom_filter : public filter
{
    Q_OBJECT
public:
    
    custom_filter(filter_class_p fc);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

    void set_name( const QString& name);
    const QString& name() const;
    
    void set_factory( filter_factory_p f );
    filter_factory_p factory();

    filter_list_p list();
    
private:
    filter_list_p    list_;
    QString name_;
};


#endif

