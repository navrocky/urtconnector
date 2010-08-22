#ifndef REGEXP_FILTER_H
#define	REGEXP_FILTER_H

#include <QPointer>
#include <QWidget>
#include <QRegExp>

#include "pointers.h"
#include "filter.h"
#include "../server_info.h"

class QLineEdit;
class QToolButton;

class regexp_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    regexp_filter_quick_opt_widget(filter_p f);

protected:
    void timerEvent(QTimerEvent* e);

private slots:
    void filter_changed();
    void text_changed();
    
private:
    QLineEdit* edit_;
    QToolButton* clear_btn_;
    filter_p filter_;
    bool block_filter_change_;
    bool block_text_change_;
    int timer_;
};

class regexp_filter_class : public filter_class
{
public:
    regexp_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f);
    virtual filter_p create_filter();

    static const char* get_id();
};

class regexp_filter : public filter
{
    Q_OBJECT
public:
    regexp_filter(filter_class_p fc);

    QString regexp() const {return rx_.pattern();}
    void set_regexp(const QString& rx);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    QRegExp rx_;
};

#endif	/* REGEXP_FILTER_H */
