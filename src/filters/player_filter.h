#ifndef PLAYER_FILTER_H
#define	PLAYER_FILTER_H

#include <QPointer>
#include <QWidget>
#include <QRegExp>

#include <common/server_info.h>
#include "pointers.h"
#include "filter.h"

class QButtonLineEdit;
class QCheckBox;

class player_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    player_filter_quick_opt_widget(filter_p f, QWidget* parent);

protected:
    void timerEvent(QTimerEvent* e);

private slots:
    void filter_changed();
    void text_changed();
    void use_rx_changed();
    
private:
    QButtonLineEdit* edit_;
    QCheckBox* use_rx_check_;
    filter_p filter_;
    bool block_filter_change_;
    bool block_text_change_;
    int timer_;
};

class player_filter_class : public filter_class
{
public:
    player_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};

class player_filter : public filter
{
    Q_OBJECT
public:
    player_filter(filter_class_p fc);

    QString pattern() const {return pattern_;}
    void set_pattern(const QString&);
    bool use_rx() const {return use_rx_;}
    void set_use_rx(bool);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    QString pattern_;
    QRegExp rx_;
    bool use_rx_;
    bool rx_valid_;
};

#endif	/* player_filter_H */
