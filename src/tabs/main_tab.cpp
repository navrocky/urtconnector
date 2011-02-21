#include <QMainWindow>
//#include <QDockWidget>
//#include <QToolBar>
//#include <QAction>
//#include <QBoxLayout>
//#include <QPointer>
#include <QSettings>
//#include <QEvent>

#include <common/server_list.h>
#include <common/qaccumulatingconnection.h>
#include <common/state_settings.h>

//#include "tools.h"
#include "main_tab.h"

main_tab::main_tab(tab_settings_p st, const tab_context& ctx, QWidget* parent)
: QMainWindow(parent)
, st_(st)
, ctx_(ctx)
{
    setObjectName(st->object_name());
    setWindowFlags(windowFlags() & (~Qt::Window));

    updater_ = new QAccumulatingConnection(ctx_.serv_list().get(), SIGNAL(changed()),
                                           this, SLOT(server_list_changed()),
                                           500, QAccumulatingConnection::Periodically, this);
//    updater_->emitSignal();
}

void main_tab::load_state()
{
    restoreState(st_->load_state(), 1);
}

void main_tab::save_state()
{
    st_->save_state(saveState(1));
}

//void main_tab::update_servers()
//{
//    updater_->emitSignal();
//}

//void main_tab::force_update_servers()
//{
//    updater_->emitNow();
//}

//const filter_list& main_tab::filterlist() const
//{
//    return *p_->filters;
//}

void main_tab::server_list_changed()
{
    emit contents_changed();
}

//void main_tab::filter_changed()
//{
//    emit contents_changed();
//}

////////////////////////////////////////////////////////////////////////////////
// tab_settings

tab_settings::tab_settings(const QString& object_name)
{
    base_settings set;
    set.register_sub_group(object_name, object_name, state_settings::uid());
    st = base_settings().get_settings(object_name);
}

void tab_settings::save_state(const QByteArray& a)
{
    st->setValue("state", a);
}

QByteArray tab_settings::load_state() const
{
    return st->value("state").toByteArray();
}
