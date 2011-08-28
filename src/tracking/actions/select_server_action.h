#ifndef TRACKING_SELECT_SERVER_ACTION_H
#define	TRACKING_SELECT_SERVER_ACTION_H

#include "../action.h"

class QWidget;

namespace tracking
{

class select_server_action_class : public action_class
{
public:
    select_server_action_class(const context_p& ctx);
    action_p create();
};

class select_server_action : public action_t
{
    Q_OBJECT
public:
    select_server_action(const action_class_p& c);

    bool execute();
    QWidget* create_options_widget(QWidget* parent);
};

}

#endif
