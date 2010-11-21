#include "ui_settings_widget.h"
#include "settings_widget.h"

namespace anticheat
{

settings_widget::settings_widget(QWidget *parent)
: settings_base_widget(parent)
, ui_(new Ui::settings_widget)
{
    ui_->setupUi(this);
}

settings_widget::~settings_widget()
{
    delete ui_;
}

void settings_widget::update_contents()
{
}

void settings_widget::apply()
{
}

}
