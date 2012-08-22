#ifndef GAME_VERSION_FILTER_H
#define	GAME_VERSION_FILTER_H

#include <QPointer>
#include <QComboBox>

#include "pointers.h"
#include "filter.h"
#include <common/server_info.h>

enum game_version_t
{
    g_41,
    g_42
};

class game_version_filter_quick_opt_widget : public QWidget
{
    Q_OBJECT
public:
    game_version_filter_quick_opt_widget(filter_p f, QWidget* parent);

private slots:
    void filter_changed();
    void combo_changed();

private:
    QComboBox* combo_;
    filter_p filter_;
    bool block_filter_change_;
    bool block_combo_change_;
};

class game_version_filter_class : public filter_class
{
public:
    game_version_filter_class();

    virtual QWidget* create_quick_opts_widget(filter_p f, QWidget* parent);
    virtual filter_p create_filter();

    static const char* get_id();
};

class game_version_filter : public filter
{
    Q_OBJECT
public:
    game_version_filter(filter_class_p fc);

    game_version_t version() const {return version_;}
    void set_version(game_version_t op);

    virtual bool filter_server(const server_info& si, filter_context& ctx);
    virtual QByteArray save();
    virtual void load(const QByteArray& ba, filter_factory_p factory);

private:
    game_version_t version_;
};

#endif

