#pragma once

#include <QWidget>

class settings_base_widget : public QWidget
{
    Q_OBJECT
public:
    settings_base_widget(QWidget *parent = 0);

    /// reimplement this to fillup setting widget
    virtual void update_contents() = 0;

    /// reimplement this to apply settings from widget
    virtual void apply() = 0;
};
