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
#include <filters/composite_filter.h>
#include <filters/hide_empty_filter.h>
#include <filters/hide_private_filter.h>
#include <filters/hide_full_filter.h>
#include <filters/ping_filter.h>
#include <filters/weapon_filter.h>
#include <filters/game_type_filter.h>
#include <filters/game_version_filter.h>

#include "../app_options.h"

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

    foreach(filter_p f, cf->filters())
    {
        correct_names(fl, f);
    }
}

}

////////////////////////////////////////////////////////////////////////////////
// filtered_tab

filtered_tab::filtered_tab(tab_settings_p st,
                           const tab_context& ctx,
                           bool complex_filter,
                           QWidget* parent)
: main_tab(st, ctx, parent)
, filters_(new filter_list(ctx.filter_factory()))
, fs_(settings())
, complex_filter_(complex_filter)
, options_valid_(false)
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
    show_filter_action_->setChecked(fs_.is_filter_visible());

    QToolBar* tb = new QToolBar(QObject::tr("Filter toolbar"), this);
    addToolBar(Qt::TopToolBarArea, tb);
    tb->setFloatable(false);
    tb->setObjectName(objectName() + "_filter_toolbar");
    tb->addAction(show_filter_action_);

    filter_holder_ = new QWidget(this);
    QBoxLayout* lay = new QHBoxLayout(filter_holder_);
    lay->setContentsMargins(0, 0, 0, 0);
    tb->addWidget(filter_holder_);

    update_toolbar_filter();

    filter_widget_ = new QDockWidget(QObject::tr("Filter"), this);
    filter_widget_->setFeatures(filter_widget_->features() ^ QDockWidget::DockWidgetFloatable);
    filter_widget_->setObjectName(objectName() + "_filter_widget");

    filter_edit_widget* filter = new filter_edit_widget(filters_, filter_widget_);
    filter_widget_->setWidget(filter);

    addDockWidget(Qt::LeftDockWidgetArea, filter_widget_);
    // FIXME restoring a dock widgets size dont work as expected
    filter_widget_->setMinimumSize(300, 0);

    connect(show_filter_action_, SIGNAL(triggered(bool)), filter_widget_, SLOT(setVisible(bool)));
    connect(filter_widget_, SIGNAL(visibilityChanged(bool)), show_filter_action_, SLOT(setChecked(bool)));
    option_check_time_.start();
}

void filtered_tab::refilter()
{
    filter_changed();
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
    }
    else
    {
        filter_holder_->setToolTip(tr("No filter"));
    }
}

void filtered_tab::save_state()
{
    main_tab::save_state();
}

void filtered_tab::load_state()
{
    main_tab::load_state();

//    restoreDockWidget(filter_widget_);

}

void filtered_tab::save_filter()
{
    fs_.save_root_filter(filters_->root_filter());

    filter_p tbf = filters_->toolbar_filter().lock();
    if (tbf)
        fs_.save_toolbar_filter(tbf->name());
    else
        fs_.save_toolbar_filter("");

    LOG_DEBUG << "%1: Filter saved", fs_.uid();
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

    if (complex_filter_)
    {
        f = filters()->create_by_class_id(ping_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);

        ping_filter* pf = dynamic_cast<ping_filter*> (f.get());
        pf->set_max(200);
        pf->set_min(0);
        pf->set_type(ping_filter::less);

        f = filters()->create_by_class_id(hide_empty_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);

        f = filters()->create_by_class_id(hide_full_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);

        f = filters()->create_by_class_id(hide_private_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);

        f = filters()->create_by_class_id(weapon_filter_class < false > ::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);

        f = filters_->create_by_class_id(composite_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        cf->add_filter(f);
        f->set_enabled(false);
        cf = dynamic_cast<composite_filter*> (f.get());
        cf->set_operation(composite_filter::op_or);

        f = filters()->create_by_class_id(game_type_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        cf->add_filter(f);
        game_type_filter* gt = dynamic_cast<game_type_filter*> (f.get());
        gt->set_mode(server_info::gm_team_survivor);

        f = filters()->create_by_class_id(game_type_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        cf->add_filter(f);
        gt = dynamic_cast<game_type_filter*> (f.get());
        gt->set_mode(server_info::gm_bomb_mode);

        f = filters()->create_by_class_id(game_type_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        cf->add_filter(f);
        gt = dynamic_cast<game_type_filter*> (f.get());
        gt->set_mode(server_info::gm_capture_the_flag);

        f = filters()->create_by_class_id(game_version_filter_class::get_id());
        f->set_name(f->get_class()->caption());
        f->set_enabled(false);
        cf->add_filter(f);
    }
}

void filtered_tab::load_filter()
{
    LOG_DEBUG << "%1: Load filter", fs_.uid();
    try
    {
        filter_p f = fs_.root_filter(filters_->factory());
        filters_->set_root_filter(f);
        correct_names(filters_, filters_->root_filter());

        QString name = fs_.toolbar_filter();
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
    filter_context ctx(false, context().friends());

    // update options
    if (!options_valid_ || option_check_time_.elapsed() > 1000)
    {
        app_settings as;
        client_version_ = as.client_version();

        option_check_time_.restart();
        options_valid_ = true;
    }

    // check for game version
    if (!client_version_.isEmpty() && si.game_type == "q3urt42")
    {
        server_info::info_t::const_iterator it = si.info.find(QString("g_modversion"));
        if (it != si.info.end())
        {
            QString version = it->second;
            if (version != client_version_)
                return false;
        }
    }

    return filters_->filtrate(si, ctx);
}

////////////////////////////////////////////////////////////////////////////////
// filtered_tab_settings

filtered_tab_settings::filtered_tab_settings(const tab_settings_p& ts)
{
    base_settings set;

    uid_ = ts->uid() + "_filtered_tab";

    set.register_sub_group(uid_, "filtered_tab", ts->uid());
    fs = base_settings().get_settings(uid_);

    //TODO backward config compatibility - remove on 0.8.0
    ts_ = ts->ts();

    update_setting_value(ts_, fs, "root_filter", "root");
    update_setting_value(ts_, fs, "toolbar_filter_name", "toolbar_filter_name");
    update_setting_value(ts_, fs, "filter_visible", "filter_visible");
}

const QString& filtered_tab_settings::uid()
{
    return uid_;
}

filter_p filtered_tab_settings::root_filter(filter_factory_p factory) const
{
    QByteArray ba = fs->value("root").toByteArray();
    return filter_load(ba, factory);
}

void filtered_tab_settings::save_root_filter(filter_p f)
{
    fs->setValue("root", filter_save(f));
}

QString filtered_tab_settings::toolbar_filter() const
{
    return fs->value("toolbar_filter_name").toString();
}

void filtered_tab_settings::save_toolbar_filter(const QString& name)
{
    fs->setValue("toolbar_filter_name", name);
}

bool filtered_tab_settings::is_filter_visible() const
{
    return fs->value("filter_visible").toBool();
}

void filtered_tab_settings::set_filter_visible(bool val)
{
    fs->setValue("filter_visible", val);
}

void filtered_tab_settings::save_state(const QByteArray& a)
{
    fs->setValue("state", a);
}

QByteArray filtered_tab_settings::load_state() const
{
    return fs->value("state").toByteArray();
}

