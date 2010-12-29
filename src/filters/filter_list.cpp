#include <algorithm>

#include "filter.h"
#include "filter_factory.h"

#include "filter_list.h"
#include "filter_edit_widget.h"
#include "composite_filter.h"

////////////////////////////////////////////////////////////////////////////////
// filter_list

filter_list::filter_list(filter_factory_p factory)
: factory_(factory)
{
}

filter_p filter_list::create_by_class_id(const QString& id) const
{
    return factory_->create_filter_by_id(id);
}

void filter_list::set_root_filter(filter_p f)
{
    root_filter_ = f;
}

void filter_list::set_toolbar_filter(filter_weak_p f)
{
    if (toolbar_filter_.lock() == f.lock())
        return;
    toolbar_filter_ = f;
    emit toolbar_filter_changed();
}

filter_p do_find_by_name(filter_p par, const QString& name)
{
    if (par->name() == name)
        return par;

    composite_filter* cf = dynamic_cast<composite_filter*>(par.get());
    if (!cf)
        return filter_p();
    foreach (filter_p f, cf->filters())
    {
        filter_p res = do_find_by_name(f, name);
        if (res)
            return res;
    }
    return filter_p();
}

filter_p filter_list::find_by_name(const QString& name)
{
    return do_find_by_name(root_filter_, name);
}

bool filter_list::is_name_correct(const QString& name)
{
    return !find_by_name(name);
}

QString filter_list::correct_name(const QString& name)
{
    QString res = name;
    int i = 1;
    while (!is_name_correct(res))
    {
        res = QString("%1 %2").arg(name).arg(i);
        i++;
    }
    return res;
}

bool filter_list::filtrate(const server_info& si) const
{
    if ( !root_filter() )
        return true;

    return root_filter()->filter_server( si );
}


