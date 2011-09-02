#ifndef TRACKING_SHOW_QUERY_ACTION_H
#define	TRACKING_SHOW_QUERY_ACTION_H

#include <QPointer>
#include <QWidget>

#include "../action.h"

class QTextEdit;
class QLineEdit;

namespace tracking
{

class show_query_action_class : public action_class
{
public:
    show_query_action_class(const context_p& ctx);
    action_p create();
};

class show_query_action : public action_t
{
    Q_OBJECT
public:
    show_query_action(const action_class_p& c);

    const QString& title() const {return title_;}
    void set_title(const QString&);
    const QString& message() const {return message_;}
    void set_message(const QString&);

    result_t execute();
    QWidget* create_options_widget(QWidget* parent);

    virtual void save(settings_t& s);
    virtual void load(const settings_t& s);

private:
    QString title_;
    QString message_;
};

class show_query_option_widget : public QWidget
{
    Q_OBJECT
public:
    show_query_option_widget(show_query_action* action, QWidget* parent);

private slots:
    void title_changed();
    void text_changed();

private:
    QLineEdit* title_edit_;
    QTextEdit* msg_edit_;
    QPointer<show_query_action> action_;
};

}

#endif	/* show_query_action_H */

