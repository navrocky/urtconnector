#pragma once

#include <common/settings_base_widget.h>

#include "manager.h"

namespace Ui
{
    class settings_widget;
}

namespace anticheat
{

class settings_widget : public settings_base_widget
{
    Q_OBJECT
public:
    settings_widget(QWidget *parent = 0);
    ~settings_widget();

    virtual void update_contents();
    virtual void apply();

private:
    Ui::settings_widget *ui_;
};

}

