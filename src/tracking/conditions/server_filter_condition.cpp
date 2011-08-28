#include "server_filter_condition.h"
#include "filters/tools.h"

#include <QObject>
#include <QTimer>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>

#include <common/qaccumulatingconnection.h>
#include <common/server_list.h>
#include <common/qt_syslog.h>
#include <updater/update_task.h>
#include <updater/update_dispatcher.h>
#include <filters/filter_edit_widget.h>
#include <filters/filter_factory.h>
#include <filters/filter_list.h>
#include <filters/composite_filter.h>

#include "../context.h"
#include "filters/reg_filters.h"

SYSLOG_MODULE(server_filter_condition)

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// server_filter_condition_class

server_filter_condition_class::server_filter_condition_class(const context_p& ctx)
: condition_class(ctx, "server_filter",
                  QObject::tr("Filter"),
                  QObject::tr("Finds server using filter"),
                  QIcon("icons:view-filter.png"))
{
}

condition_p server_filter_condition_class::create()
{
    return condition_p(new server_filter_condition(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// server_filter_condition

server_filter_condition::server_filter_condition(const condition_class_p& c)
: condition_t(c)
, update_interval_(60*1000)
, use_auto_update_(true)
{
    filter_factory_p ff(new filter_factory);
    register_filters(ff);
    filters_.reset(new filter_list(ff));
    filter_p f = ff->create_filter_by_id(composite_filter_class::get_id());
    assert(f);
    filters_->set_root_filter(f);
}

void server_filter_condition::set_interval(int val)
{
    if (val == update_interval_)
        return;
    update_interval_ = val;
    changed();
}

void server_filter_condition::set_servers(const QString& val)
{
    servers_ = val;
    changed();
}

void server_filter_condition::set_use_auto_update(bool val)
{
    if (use_auto_update_ == val)
        return;
    use_auto_update_ = val;
    changed();
}

void server_filter_condition::save(settings_t& s)
{
    condition_t::save(s);
    s["update_interval"] = interval();
    s["servers"] = servers_;
    QByteArray ba = filter_save(filters_->root_filter());
    s["filters"] = ba;
    s["auto_update"] = use_auto_update_;
}

void server_filter_condition::load(const settings_t& s)
{
    condition_t::load(s);
    settings_t::const_iterator it = s.find("update_interval");
    if (it != s.end())
        set_interval(it.value().toInt());

    it = s.find("auto_update");
    if (it != s.end())
        set_use_auto_update(it.value().toBool());

    it = s.find("servers");
    if (it != s.end())
        set_servers(it.value().toString());

    it = s.find("filters");
    if (it != s.end())
    {
        QByteArray ba = it.value().toByteArray();
        try
        {
            filter_p f = filter_load(ba, filters_->factory());
            filters_->set_root_filter(f);
        }
        catch(...)
        {
        }
    }
}

void server_filter_condition::do_start()
{
    context_p ctx = get_class()->context();

    srv_list_.clear();
    QStringList sl = servers_.split(QRegExp("[;,\\s]+"), QString::SkipEmptyParts);
    foreach (const QString& s, sl)
    {
        srv_list_.append(server_id(s));
    }

    if (use_auto_update_)
    {
        update_task_ = new update_task(this);
        update_task_->set_interval(update_interval_);
        update_task_->set_servers(srv_list_);
        update_task_->set_single_shot(false);
        ctx->update_disp->add_task(update_task_);
    }

    srv_list_changed_conn_ = new QAccumulatingConnection(ctx->srv_list.get(),
        SIGNAL(changed()), this, SLOT(srv_list_changed()), 500,
        QAccumulatingConnection::Periodically, this);
}

void server_filter_condition::do_stop()
{
    delete srv_list_changed_conn_;
    delete update_task_;
}

QWidget* server_filter_condition::create_options_widget(QWidget* parent)
{
    return new server_filter_condition_widget(parent, this);
}

void server_filter_condition::srv_list_changed()
{
    LOG_DEBUG << "Check condition";

    context_p tr_ctx = get_class()->context();

    server_list_p srv_list = tr_ctx->srv_list;

    filter_context ctx;
    ctx.data = &(tr_ctx->data);
    ctx.full_filter_process = true;

    server_id founded_server;
    if (srv_list_.isEmpty())
    {
        foreach (server_info_list::const_reference r, srv_list->list())
        {
            server_info_p si = r.second;
            if (si->status == server_info::s_up && !si->updating && filters_->filtrate(*si, ctx))
            {
                founded_server = r.first;
                break;
            }
        }
    } else
    {
        foreach (const server_id& id, srv_list_)
        {
            server_info_p si = srv_list->get(id);
            if (si->status == server_info::s_up && !si->updating && filters_->filtrate(*si, ctx))
            {
                founded_server = id;
                break;
            }
        }
    }

    if (!founded_server.is_empty())
    {
        QString srv_id = founded_server.address();
        QString srv_name;
        tr_ctx->data.insert("server_id", srv_id);
        server_info_list::const_iterator it = srv_list->list().find(founded_server);
        if (it != srv_list->list().end())
        {
            srv_name = it->second->name;
            tr_ctx->data.insert("server_name", srv_name);
        }
        if (srv_name.isEmpty())
            tr_ctx->data.insert("server", srv_id);
        else
            tr_ctx->data.insert("server", QString("%1 (%2)").arg(srv_name).arg(srv_id));

        // condition arised
        trigger();
    }
}

////////////////////////////////////////////////////////////////////////////////
// server_filter_condition_widget

server_filter_condition_widget::server_filter_condition_widget(QWidget* parent, server_filter_condition* cond)
: QWidget(parent)
, cond_(cond)
{
    QVBoxLayout* vl = new QVBoxLayout(this);
    vl->setContentsMargins(0, 0, 0, 0);
    QFormLayout* l = new QFormLayout;
    vl->addLayout(l);

    auto_update_check_ = new QCheckBox(this);
    auto_update_check_->setText(tr("Use auto update"));
    auto_update_check_->setChecked(cond_->use_auto_update());
    connect(auto_update_check_, SIGNAL(stateChanged(int)), SLOT(auto_update_check_changed()));
    l->addRow(auto_update_check_);

    spin_ = new QDoubleSpinBox(this);
    spin_->setMinimum(0.1);
    spin_->setMaximum(1000);
    spin_->setSuffix(tr(" sec"));
    spin_->setSingleStep(1);
    spin_->setValue(cond->interval() / 1000);
    l->addRow(tr("Update interval"), spin_);
    connect(spin_, SIGNAL(valueChanged(double)), SLOT(spin_changed(double)));

    servers_edit_ = new QLineEdit(this);
    servers_edit_->setToolTip(tr("Server address list delimited by \" \", \",\" or \":\""));
    l->addRow(tr("Servers"), servers_edit_);
    connect(servers_edit_, SIGNAL(editingFinished()), SLOT(srv_list_edit_finished()));
    servers_edit_->setText(cond->servers());

    filter_panel_ = new filter_edit_widget(cond_->filters(), this);
    filter_panel_->set_show_snap(false);
    vl->addWidget(filter_panel_);
    update_auto_update();
}

void server_filter_condition_widget::spin_changed(double val)
{
    cond_->set_interval(val * 1000);
}

void server_filter_condition_widget::srv_list_edit_finished()
{
    const QString& servers = servers_edit_->text();
    
    // check list syntax
    server_id_list srv_list;
    QStringList sl = servers.split(QRegExp("[;,\\s]+"), QString::SkipEmptyParts);
    foreach (const QString& s, sl)
    {
        srv_list.append(server_id(s));
    }

    cond_->set_servers(servers);
}

void server_filter_condition_widget::auto_update_check_changed()
{
    cond_->set_use_auto_update(auto_update_check_->isChecked());
    update_auto_update();
}

void server_filter_condition_widget::update_auto_update()
{
    bool fl = auto_update_check_->isChecked();
    spin_->setEnabled(fl);
}

}
