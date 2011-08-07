#include "update_task.h"

update_task::update_task(QObject* parent)
: QObject(parent)
, single_shot_(false)
, interval_(60*1000)
, is_finished_(false)
{
}

void update_task::set_single_shot(bool val)
{
    if (single_shot_ == val)
        return;
    single_shot_ = val;
}

void update_task::set_interval(int val)
{
    if (interval_ == val)
        return;
    interval_ = val;
    emit interval_changed();
}

void update_task::set_servers(const server_id_list& val)
{
    servers_ = val;
}
