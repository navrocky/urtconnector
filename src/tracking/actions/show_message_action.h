#ifndef TRACKING_SHOW_MESSAGE_ACTION_H
#define	TRACKING_SHOW_MESSAGE_ACTION_H

#include <QPointer>
#include <QWidget>

#include "../action.h"

class QTextEdit;
class QLineEdit;

namespace tracking
{

class show_message_action_class : public action_class
{
public:
    show_message_action_class(const context_p& ctx);
    action_p create();
};

class show_message_action : public action_t
{
    Q_OBJECT
public:
    show_message_action(const action_class_p& c);

    const QString& message() const {return message_;}
    void set_message(const QString&);

    bool execute();
    QWidget* create_options_widget(QWidget* parent);

    virtual void save(settings_t& s);
    virtual void load(const settings_t& s);

private:
    QString message_;
};

class show_message_option_widget : public QWidget
{
    Q_OBJECT
public:
    show_message_option_widget(show_message_action* action, QWidget* parent);

private slots:
    void text_changed();

private:
    QTextEdit* msg_edit_;
    QPointer<show_message_action> action_;
};

}

#endif	/* SHOW_MESSAGE_ACTION_H */

