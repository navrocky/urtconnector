#include "connect_action.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QMessageBox>
#include <QApplication>

#include <common/server_connect_iface.h>
#include <common/scoped_tools.h>
#include "../context.h"
#include "../tools.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// connect_action_class

connect_action_class::connect_action_class(const context_p& ctx)
: action_class(ctx, "connect", QObject::tr("Connect to server"),
               QObject::tr("Connects to the specified server or to the server provided by condition."),
               QIcon("icons:launch.png"))
{

}

action_p connect_action_class::create()
{
    return action_p(new connect_action(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// connect_action

connect_action::connect_action(const action_class_p& c)
: action_t(c)
, block_execute_(false)
{
}

void connect_action::set_server(const server_id& id)
{
    if (server_ == id)
        return;
    server_ = id;
    emit changed();
}

action_t::result_t connect_action::execute()
{
    if (block_execute_)
        return r_cancel;

    SCOPE_COCK_FLAG(block_execute_);

    context_p ctx = get_class()->context();

    server_id id = server_;
    if (id.is_empty())
    {
        data_map_t::iterator it = ctx->data.find("server_id");
        if (it != ctx->data.end())
        {
            QString s = it.value();
            id = server_id(s);
        }
    }

    if (id.is_empty())
        return r_cancel;

    ctx->connect_iface->connect_to_server(id, QString(), QString(), false);
    return r_continue;
}

QWidget* connect_action::create_options_widget(QWidget* parent)
{
    return new connect_option_widget(this, parent);
}

void connect_action::save(settings_t& s)
{
    s["server"] = server_.address();
}

void connect_action::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("server");
    if (it != s.end())
        set_server(server_id(it.value().toString()));
}


////////////////////////////////////////////////////////////////////////////////
// connect_option_widget

connect_option_widget::connect_option_widget(connect_action* action, QWidget* parent)
: QWidget(parent)
, action_(action)
{
    QFormLayout* l = new QFormLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    server_edit_ = new QLineEdit(this);
    server_edit_->setToolTip(tr("Server address to connect. "
        "If the field is empty connects to the server from the condition."));
    l->addRow(server_edit_);
    
    server_id id = action_->server();
    if (!id.is_empty())
        server_edit_->setText(id.address());

    connect(server_edit_, SIGNAL(textChanged(const QString&)), SLOT(server_changed()));
}

void connect_option_widget::server_changed()
{
    action_->set_server(server_id(server_edit_->text()));
}

}

