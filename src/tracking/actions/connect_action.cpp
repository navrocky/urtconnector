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
, query_before_connect_(true)
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

void connect_action::set_query_before_connect(bool val)
{
    if (query_before_connect_ == val)
        return;
    query_before_connect_ = val;
    emit changed();
}

void connect_action::set_message(const QString& msg)
{
    if (msg_ == msg)
        return;
    msg_ = msg;
    emit changed();
}

void connect_action::execute()
{
    if (block_execute_)
        return;

    SCOPE_COCK_FLAG(block_execute_);

    context_p ctx = get_class()->context();

    server_id id = server_;
    if (id.is_empty())
    {
        data_map_t::iterator it = ctx->data.find("server");
        if (it != ctx->data.end())
        {
            QString s = it.value();
            id = server_id(s);
        }
    }

    if (id.is_empty())
        return;

    if (query_before_connect_)
    {
        QString msg = msg_;
        if (msg.isEmpty())
            msg = tr("Connection action triggered.\nProcess to connect to the server \"%server\" ?");

        replace_msg_tags(msg_, ctx->data);
        int res = QMessageBox::question(0, tr("Connect to the server"),
                                        msg_, QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
        if (res != QMessageBox::Yes)
            return;
    }

    ctx->connect_iface->connect_to_server(id, QString(), QString(), false);
}

QWidget* connect_action::create_options_widget(QWidget* parent)
{
    return new connect_option_widget(this, parent);
}

void connect_action::save(settings_t& s)
{
    s["query_before_connect"] = query_before_connect_;
    s["server"] = server_.address();
    s["message"] = msg_;
}

void connect_action::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("query_before_connect");
    if (it != s.end())
        set_query_before_connect(it.value().toBool());
    it = s.find("server");
    if (it != s.end())
        set_server(server_id(it.value().toString()));
    it = s.find("message");
    if (it != s.end())
        set_message(it.value().toString());
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
    l->addRow(tr("Server"), server_edit_);
    
    query_check_ = new QCheckBox(this);
    query_check_->setText(tr("Query before connect"));
    l->addRow(query_check_);

    msg_edit_ = new QLineEdit(this);
    msg_edit_->setToolTip(tr("Query message displayed to the user before connecting."));
    l->addRow(tr("Query"), msg_edit_);

    server_id id = action_->server();
    if (!id.is_empty())
        server_edit_->setText(id.address());
    query_check_->setChecked(action_->query_before_connect());
    msg_edit_->setText(action_->message());

    connect(query_check_, SIGNAL(stateChanged(int)), SLOT(query_checked()));
    connect(server_edit_, SIGNAL(textChanged(const QString&)), SLOT(server_changed()));
    connect(msg_edit_, SIGNAL(textChanged(const QString&)), SLOT(msg_changed()));
}

void connect_option_widget::server_changed()
{
    action_->set_server(server_id(server_edit_->text()));
}

void connect_option_widget::query_checked()
{
    action_->set_query_before_connect(query_check_->isChecked());
}

void connect_option_widget::msg_changed()
{
    action_->set_message(msg_edit_->text());
}

}

