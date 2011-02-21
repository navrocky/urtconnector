#include <QSettings>
#include <QAction>
#include <QToolBar>
#include <QHBoxLayout>
#include <QEvent>
#include <QDockWidget>

#include <common/qt_syslog.h>
#include <filters/filter_list.h>
#include <filters/composite_filter.h>
#include <filters/filter_edit_widget.h>
#include <filters/regexp_filter.h>
#include <filters/tools.h>

#include "filtered_tab.h"

SYSLOG_MODULE(filtered_tab)

namespace
{

void correct_names(filter_list_p fl, filter_p par)
{
    if (par->name().isEmpty())
        par->set_name(fl->correct_name(par->get_class()->caption()));

    composite_filter* cf = dynamic_cast<composite_filter*> (par.get());
    if (!cf)
        return;
    foreach (filter_p f, cf->filters())
    {
        correct_names(fl, f);
    }
}

}

////////////////////////////////////////////////////////////////////////////////
// filtered_tab

filtered_tab::filtered_tab(tab_settings_p st, const tab_context& ctx, QWidget* parent)
: main_tab(st, ctx, parent)
, filters_(new filter_list(ctx.filter_factory()))
, st_(dynamic_cast<filtered_tab_settings*>(st.get()))
{
    load_filter();

    connect(filters_->root_filter().get(), SIGNAL(changed_signal()),
            this, SLOT(filter_changed()));

    connect(filters_.get(), SIGNAL(toolbar_filter_changed()),
            this, SLOT(update_toolbar_filter()));

    connect(filters_->root_filter().get(), SIGNAL(changed_signal()),
            this, SLOT(save_filter()));

    connect(filters_.get(), SIGNAL(toolbar_filter_changed()),
            this, SLOT(save_filter()));

    show_filter_action_ = new QAction(QIcon("icons:view-filter.png"), tr("View and edit filter"), this);
    show_filter_action_->setCheckable(true);
    show_filter_action_->setChecked(st_->is_filter_visible());

    QToolBar* tb = new QToolBar(QObject::tr("Filter toolbar"), this);
    addToolBar(Qt::TopToolBarArea, tb);
    tb->setFloatable(false);
    tb->setObjectName("filter_toolbar");
    tb->addAction(show_filter_action_);

    filter_holder_ = new QWidget(this);
    QBoxLayout* lay = new QHBoxLayout(filter_holder_);
    lay->setContentsMargins(0, 0, 0, 0);
    tb->addWidget(filter_holder_);

    update_toolbar_filter();

    filter_widget_ = new QDockWidget(QObject::tr("Filter"), this);
    filter_widget_->setFeatures(filter_widget_->features() ^ QDockWidget::DockWidgetFloatable);
    filter_widget_->setObjectName("filter_widget");

    filter_edit_widget* filter = new filter_edit_widget(filters_, filter_widget_);
    filter_widget_->setWidget(filter);

    addDockWidget(Qt::LeftDockWidgetArea, filter_widget_);

    connect(show_filter_action_, SIGNAL(triggered(bool)), filter_widget_, SLOT(setVisible(bool)));
    connect(filter_widget_, SIGNAL(visibilityChanged(bool)), show_filter_action_, SLOT(setChecked(bool)));
}

void filtered_tab::update_toolbar_filter()
{
    // remove old
    delete filter_toolbar_widget_;

    // create new
    filter_p f = filters_->toolbar_filter().lock();
    if (f)
    {
        QWidget* w = f->get_class()->create_quick_opts_widget(f, filter_holder_);
        if (w)
            filter_holder_->layout()->addWidget(w);
        filter_holder_->setToolTip(f->get_class()->caption());
        filter_toolbar_widget_ = w;
    } else
    {
        filter_holder_->setToolTip(tr("No filter"));
    }
}

void filtered_tab::save_state()
{
    main_tab::save_state();
    st_->set_filter_visible(filter_widget_->isVisible());
}

void filtered_tab::load_state()
{
    main_tab::load_state();
    filter_widget_->setVisible(st_->is_filter_visible());
}

void filtered_tab::save_filter()
{
    st_->save_root_filter(filters_->root_filter());

    filter_p tbf = filters_->toolbar_filter().lock();
    if (tbf)
        st_->save_toolbar_filter(tbf->name());
    else
        st_->save_toolbar_filter("");

    LOG_DEBUG << "%1: Filter saved", st_->object_name();
}

void filtered_tab::default_filter_initialization()
{
    // default filter initialization
    filter_p f = filters_->create_by_class_id(composite_filter_class::get_id());
    f->set_name(f->get_class()->caption());
    filters_->set_root_filter(f);
    composite_filter* cf = dynamic_cast<composite_filter*> (f.get());

    // create regexp filter as composite child
    f = filters_->create_by_class_id(regexp_filter_class::get_id());
    f->set_name(f->get_class()->caption());
    cf->add_filter(f);

    // select regexp filter for toolbar
    filters_->set_toolbar_filter(f);
}

void filtered_tab::load_filter()
{
    LOG_DEBUG << "%1: Load filter", st_->object_name();
    try
    {
        filter_p f = st_->root_filter(filters_->factory());
        filters_->set_root_filter(f);
        correct_names(filters_, filters_->root_filter());

        QString name = st_->toolbar_filter();
        f = filters_->find_by_name(name);
        filters_->set_toolbar_filter(f);
    }
    catch (const std::exception&)
    {
        default_filter_initialization();
    }
}

void filtered_tab::filter_changed()
{
}

bool filtered_tab::filtrate(const server_info& si) const
{
    return filters_->filtrate(si);
}

////////////////////////////////////////////////////////////////////////////////
// filtered_tab_settings

filtered_tab_settings::filtered_tab_settings(const QString& object_name)
: tab_settings(object_name)
{
}

filter_p filtered_tab_settings::root_filter(filter_factory_p factory) const
{
    QByteArray ba = st->value("root_filter").toByteArray();
    return filter_load(ba, factory);
}

void filtered_tab_settings::save_root_filter(filter_p f)
{
    st->setValue("root_filter", filter_save(f));
}

QString filtered_tab_settings::toolbar_filter() const
{
    return st->value("toolbar_filter_name").toString();
}

void filtered_tab_settings::save_toolbar_filter(const QString& name)
{
    st->setValue("toolbar_filter_name", name);
}

bool filtered_tab_settings::is_filter_visible() const
{
    return st->value("filter_visible").toBool();
}

void filtered_tab_settings::set_filter_visible(bool val)
{
    st->setValue("filter_visible", val);
}
