#ifndef ONLINE_CLANS_FILTER_H
#define	ONLINE_CLANS_FILTER_H

#include <QPointer>
#include <QWidget>

#include "pointers.h"
#include "filter.h"
#include "../server_info.h"

class QSpinBox;

class online_clans_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    online_clans_filter_quick_opt_widget(filter_p f);
private slots:
    void filter_changed();
    void tag_length_changed();
    void players_changed();
private:
    filter_p filter_;
    QSpinBox* tag_length_spin_;
    QSpinBox* players_spin_;
    bool block_filter_change_;
    bool block_ctl_change_;
};

class online_clans_filter_class : public filter_class
{
public:
    online_clans_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f);
    virtual filter_p create_filter();

    static const char* get_id();
};

class online_clans_filter : public filter
{
    Q_OBJECT
public:
    online_clans_filter(filter_class_p fc);

    int minimal_tag_length() const {return minimal_tag_length_;}
    void set_minimal_tag_length(int val);

    int minimal_players() const {return minimal_players_;}
    void set_minimal_players(int val);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    int minimal_tag_length_;
    int minimal_players_;
};

#endif

