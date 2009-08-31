#include "servlistwidget.h"

ServListWidget::ServListWidget(QWidget *parent)
 : QWidget(parent)
{
    ui_.setupUi(this);
}


ServListWidget::~ServListWidget()
{
}

void ServListWidget::setServerList(ServerListCustom * ptr)
{
    servList_ = ptr;
}


