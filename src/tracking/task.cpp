#include "task.h"

#include <cassert>

#include <common/scoped_tools.h>
#include "action.h"
#include "condition.h"

namespace tracking
{

task_t::task_t(QObject* parent)
: QObject(parent)
, op_mode_(om_multi_trigger)
, block_changed_(false)
, block_execute_(false)
{
}

void task_t::set_id(const QString& id)
{
    id_ = id;
}

void task_t::set_operation_mode(operation_mode_t op)
{
    if (op_mode_ == op)
        return;
    op_mode_ = op;
    do_changed();
}

void task_t::set_condition(const condition_p& cond)
{
    if (cond_)
    {
        disconnect(cond_.get(), SIGNAL(changed()), this, SIGNAL(changed()));
        disconnect(cond_.get(), SIGNAL(triggered()), this, SLOT(condition_triggered()));
    }
    cond_ = cond;
    if (cond_)
    {
        connect(cond_.get(), SIGNAL(triggered()), this, SLOT(condition_triggered()));
        connect(cond_.get(), SIGNAL(changed()), this, SIGNAL(changed()));
    }
    do_changed();
}

void task_t::add_action(const action_p& a)
{
    actions_.append(a);
    connect(a.get(), SIGNAL(changed()), this, SIGNAL(changed()));
    do_changed();
}

void task_t::remove_action(const action_p& a)
{
    disconnect(a.get(), SIGNAL(changed()), this, SIGNAL(changed()));
    actions_.removeAll(a);
    do_changed();
}

int task_t::move_action(const action_p& a, int delta)
{
    int i = actions_.indexOf(a);
    int newpos = i + delta;
    if (newpos < 0)
        newpos = 0;
    if (newpos > actions_.size() - 1)
        newpos = actions_.size() - 1;
    actions_.move(i, newpos);
    return newpos - i;
}

void task_t::condition_triggered()
{
    if (block_execute_)
        return;
    SCOPE_COCK_FLAG(block_execute_);

    action_t::result_t res = action_t::r_continue;
    foreach (const action_p& a, actions_)
    {
        res = a->execute();
        if (res != action_t::r_continue)
            break;
    }
    
    if (res == action_t::r_skip)
    {
        cond_->skip_current();
        return;
    }

    switch (op_mode_)
    {
        case om_multi_trigger:
            break;
        case om_single_trigger:
            cond_->stop();
            break;
        case om_destroy_after_trigger:
            cond_->stop();
            deleteLater();
            break;
    }
}

void task_t::assign(task_t* src)
{
    {
        SCOPE_COCK_FLAG(block_changed_);

        id_ = src->id_;
        caption_ = src->caption_;
        op_mode_ = src->op_mode_;

        if (src->condition())
        {
            condition_p c = src->condition()->get_class()->create();
            c->assign(src->condition().get());
            set_condition(c);
        } else
        {
            set_condition(condition_p());
        }

        foreach (const action_p& a, src->actions())
        {
            disconnect(a.get(), SIGNAL(changed()), this, SIGNAL(changed()));
        }
        actions_.clear();

        foreach (const action_p& sa, src->actions())
        {
            action_p a = sa->get_class()->create();
            a->assign(sa.get());
            add_action(a);
        }
    }
    do_changed();
}

void task_t::save(settings_t& s)
{
    s["id"] = id_;
    s["caption"] = caption_;
    s["op_mode"] = int(op_mode_);

    if (condition())
    {
        s["condition_class"] = condition()->get_class()->id();
        settings_t cs;
        condition()->save(cs);
        s["condition"] = cs;
    }

    s["action_count"] = actions_.size();
    for (int i = 0; i < actions_.size(); i++)
    {
        const action_p& a = actions_[i];
        const QString& ac = a->get_class()->id();
        s[QString("action_class_%1").arg(i)] = ac;
        settings_t as;
        a->save(as);
        s[QString("action_%1").arg(i)] = as;
    }
}

void task_t::load(const settings_t& s, const condition_factory_p& conds,
        const action_factory_p& acts)
{
    {
        SCOPE_COCK_FLAG(block_changed_);

        id_ = s["id"].toString();
        caption_ = s["caption"].toString();
        op_mode_ = operation_mode_t(s["op_mode"].toInt());

        QString cc = s["condition_class"].toString();
        if (!cc.isEmpty())
        {
            condition_p cond = conds->create(cc);
            settings_t cs = s["condition"].value<settings_t>();
            cond->load(cs);
            set_condition(cond);
        }
        int action_count = s["action_count"].toInt();
        for (int i = 0; i < action_count; i++)
        {
            QString ac = s[QString("action_class_%1").arg(i)].toString();
            action_p a = acts->create(ac);
            settings_t as = s[QString("action_%1").arg(i)].value<settings_t>();
            a->load(as);
            add_action(a);
        }
    }
    do_changed();
}


void task_t::do_changed()
{
    if (!block_changed_)
        emit changed();
}

}
