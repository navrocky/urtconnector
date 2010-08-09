#ifndef GAME_TYPE_FILTER_H
#define	GAME_TYPE_FILTER_H

#include <QPointer>
#include <QComboBox>

#include "pointers.h"
#include "filter.h"
#include "../server_info.h"

class game_type_filter_quick_opt_widget : public QComboBox
{
    Q_OBJECT
public:
    game_type_filter_quick_opt_widget(filter_p f);
private slots:
    void filter_changed();
    void combo_changed();
private:
    filter_p filter_;
    bool block_filter_change_;
    bool block_combo_change_;
};

class game_type_filter_class : public filter_class
{
public:
    game_type_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f);
    virtual filter_p create_filter();

    static const char* get_id();
};

class game_type_filter : public filter
{
    Q_OBJECT
public:
    game_type_filter(filter_class_p fc);

    server_info::game_mode mode() const {return mode_;}
    void set_mode(server_info::game_mode op);

    virtual bool filter_server(const server_info& si);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    server_info::game_mode mode_;
};

#endif	/* GAME_TYPE_FILTER_H */

