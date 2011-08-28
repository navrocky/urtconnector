#include "show_message_action.h"

#include <QHBoxLayout>
#include <QTextEdit>
#include <QMessageBox>

#include "../context.h"
#include "../tools.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// show_message_action_class

show_message_action_class::show_message_action_class(const context_p& ctx)
: action_class(ctx, "show_message", QObject::tr("Show message"),
               QObject::tr("Show message to the user if an action triggered"),
               QIcon("icons:help-about.png"))
{

}

action_p show_message_action_class::create()
{
    return action_p(new show_message_action(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// show_message_action

show_message_action::show_message_action(const action_class_p& c)
: action_t(c)
{
}

bool show_message_action::execute()
{
    QString s = message_;
    replace_msg_tags(s, get_class()->context()->data);
    int res = QMessageBox::information(0, tr("Information"), s, QMessageBox::Ok);
    return res == QMessageBox::Ok;
}

QWidget* show_message_action::create_options_widget(QWidget* parent)
{
    return new show_message_option_widget(this, parent);
}

void show_message_action::save(settings_t& s)
{
    s["message"] = message_;
}

void show_message_action::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("message");
    if (it != s.end())
        set_message(it.value().toString());
}

void show_message_action::set_message(const QString& msg)
{
    if (message_ == msg)
        return;
    message_ = msg;
    emit changed();
}


////////////////////////////////////////////////////////////////////////////////
// show_message_option_widget

show_message_option_widget::show_message_option_widget(show_message_action* action, QWidget* parent)
: QWidget(parent)
, action_(action)
{
    QHBoxLayout* l = new QHBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    msg_edit_ = new QTextEdit(this);
    connect(msg_edit_, SIGNAL(textChanged()), SLOT(text_changed()));
    l->addWidget(msg_edit_);
    msg_edit_->setText(action_->message());
    msg_edit_->setFixedHeight(100);
}

void show_message_option_widget::text_changed()
{
    action_->set_message(msg_edit_->toPlainText());
}


}

