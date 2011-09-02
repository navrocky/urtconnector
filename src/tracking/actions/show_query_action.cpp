#include "show_query_action.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QTextEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QStyle>

#include "../context.h"
#include "../tools.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// show_query_action_class

show_query_action_class::show_query_action_class(const context_p& ctx)
: action_class(ctx, "show_query", QObject::tr("Show question"),
               QObject::tr("Show question to the user if an action triggered.\n"
                           "If the user selects \"No\" then processing of following actions stops."),
               QIcon("icons:question.png"))
{
}

action_p show_query_action_class::create()
{
    return action_p(new show_query_action(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// show_query_action

show_query_action::show_query_action(const action_class_p& c)
: action_t(c)
{
}

action_t::result_t show_query_action::execute()
{
    QString title = title_;
    replace_msg_tags(title, get_class()->context()->data);
    if (title_.isEmpty())
        title_ = tr("Question");

    QString msg = message_;
    replace_msg_tags(msg, get_class()->context()->data);

    QMessageBox mb;
    mb.setWindowTitle(title);
    mb.setText(msg);
    mb.setIcon(QMessageBox::Question);
    QPushButton* yes_btn = mb.addButton(QMessageBox::Yes);
    QPushButton* no_btn = mb.addButton(QMessageBox::No);
    QPushButton* skip_btn = mb.addButton(tr("Skip"), QMessageBox::RejectRole);
    skip_btn->setIcon(mb.style()->standardIcon(QStyle::SP_DialogResetButton));
    mb.exec();
    result_t res;
    if (mb.clickedButton() == yes_btn)
        return r_continue;
    else if (mb.clickedButton() == no_btn)
        return r_cancel;
    else if (mb.clickedButton() == skip_btn)
        return r_skip;
    else
        assert(false);
    return r_cancel;
}

QWidget* show_query_action::create_options_widget(QWidget* parent)
{
    return new show_query_option_widget(this, parent);
}

void show_query_action::save(settings_t& s)
{
    s["title"] = title_;
    s["message"] = message_;
}

void show_query_action::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("title");
    if (it != s.end())
        set_title(it.value().toString());
    it = s.find("message");
    if (it != s.end())
        set_message(it.value().toString());
}

void show_query_action::set_title(const QString& val)
{
    if (title_ == val)
        return;
    title_ = val;
    emit changed();
}

void show_query_action::set_message(const QString& msg)
{
    if (message_ == msg)
        return;
    message_ = msg;
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
// show_query_option_widget

show_query_option_widget::show_query_option_widget(show_query_action* action, QWidget* parent)
: QWidget(parent)
, action_(action)
{
    QFormLayout* l = new QFormLayout(this);
    l->setContentsMargins(0, 0, 0, 0);
    title_edit_ = new QLineEdit(this);
    connect(title_edit_, SIGNAL(textChanged(const QString&)), SLOT(title_changed()));
    l->addRow(tr("Title"), title_edit_);

    msg_edit_ = new QTextEdit(this);
    connect(msg_edit_, SIGNAL(textChanged()), SLOT(text_changed()));
    l->addRow(msg_edit_);
    msg_edit_->setFixedHeight(100);
    title_edit_->setText(action_->title());
    msg_edit_->setText(action_->message());
}

void show_query_option_widget::title_changed()
{
    action_->set_title(title_edit_->text());
}

void show_query_option_widget::text_changed()
{
    action_->set_message(msg_edit_->toPlainText());
}


}

