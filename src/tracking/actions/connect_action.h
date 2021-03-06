#ifndef TRACKING_CONNECT_ACTION_H
#define	TRACKING_CONNECT_ACTION_H

#include <QPointer>
#include <QWidget>

#include <common/server_id.h>
#include "../action.h"

class QLineEdit;
class QCheckBox;

namespace tracking
{

class connect_action_class : public action_class
{
public:
    connect_action_class(const context_p& ctx);
    action_p create();
};

class connect_action : public action_t
{
    Q_OBJECT
public:
    connect_action(const action_class_p& c);

    server_id server() const {return server_;}
    void set_server(const server_id& id);

    result_t execute();
    QWidget* create_options_widget(QWidget* parent);

    virtual void save(settings_t& s);
    virtual void load(const settings_t& s);

private:
    server_id server_;
    bool block_execute_;
};

class connect_option_widget : public QWidget
{
    Q_OBJECT
public:
    connect_option_widget(connect_action* action, QWidget* parent);

private slots:
    void server_changed();

private:
    QLineEdit* server_edit_;
    QPointer<connect_action> action_;
};

}

#endif
