#ifndef TRACKING_SERVER_FILTER_CONDITION_H
#define	TRACKING_SERVER_FILTER_CONDITION_H

#include <set>
#include <QWidget>
#include <QPointer>

#include <common/server_id.h>
#include <filters/pointers.h>

#include "../condition.h"

class QTimer;
class QDoubleSpinBox;
class QLineEdit;
class QCheckBox;
class QAccumulatingConnection;
class update_task;
class filter_edit_widget;

namespace tracking
{

class server_filter_condition_class : public condition_class
{
public:
    server_filter_condition_class(const context_p& ctx);
    condition_p create();
};


class server_filter_condition : public condition_t
{
    Q_OBJECT
public:
    server_filter_condition(const condition_class_p& c);

    int interval() const {return update_interval_;}
    void set_interval(int);
    const QString& servers() const {return servers_;}
    void set_servers(const QString&);
    filter_list_p filters() const {return filters_;}

    bool use_auto_update() const {return use_auto_update_;}
    void set_use_auto_update(bool);

    void save(settings_t& s);
    void load(const settings_t& s);

    QWidget* create_options_widget(QWidget* parent);

    void skip_current();

private slots:
    void srv_list_changed();

protected:
    void do_start();
    void do_stop();

private:
    void server_found(const server_id&);

    int update_interval_;
    bool use_auto_update_;
    QString servers_;
    server_id_list srv_list_;
    filter_list_p filters_;
    QPointer<update_task> update_task_;
    QPointer<QAccumulatingConnection> srv_list_changed_conn_;
    server_id founded_server_;
    typedef std::set<server_id> servers_set_t;
    servers_set_t skipped_servers_;
};

class server_filter_condition_widget : public QWidget
{
    Q_OBJECT
public:
    server_filter_condition_widget(QWidget* parent, server_filter_condition* cond);

private slots:
    void spin_changed(double);
    void srv_list_edit_finished();
    void auto_update_check_changed();

private:
    void update_auto_update();


    QDoubleSpinBox* spin_;
    QLineEdit* servers_edit_;
    QCheckBox* auto_update_check_;
    filter_edit_widget* filter_panel_;
    QPointer<server_filter_condition> cond_;
};

}

#endif
