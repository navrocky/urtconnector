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

main_tab::main_tab(const tab_settings_p& st, const tab_context& ctx, QWidget* parent)
    : QMainWindow(parent)
    , st_(st)
    , ctx_(ctx)
{
    setObjectName( st->uid() );
    setWindowFlags(windowFlags() & (~Qt::Window));

    updater_ = new QAccumulatingConnection(ctx_.serv_list().get(), SIGNAL(changed()),
                                           this, SLOT(server_list_changed()),
                                           500, QAccumulatingConnection::Periodically, this);
//    updater_->emitSignal();
}

void main_tab::load_state()
{
    restoreGeometry( st_->load_geometry() );
    restoreState(st_->load_state(), 1);
}

void main_tab::save_state()
{
    st_->save_geometry( saveGeometry() );
    st_->save_state(saveState(1));
}

void main_tab::server_list_changed()
{
    emit contents_changed();
}

////////////////////////////////////////////////////////////////////////////////
// tab_settings

tab_settings::tab_settings(const QString& name)
    :uid_(name)
{
    base_settings set;
    set.register_sub_group( uid_, uid_, state_settings::uid() );
    st = base_settings().get_settings(uid_);
}

void tab_settings::save_state(const QByteArray& a)
{
    st->setValue("state", a);
}

QByteArray tab_settings::load_state() const
{
    return st->value("state").toByteArray();
}

void tab_settings::save_geometry(const QByteArray& a)
{
    st->setValue("geometry", a);
}

QByteArray tab_settings::load_geometry() const
{
    return st->value("geometry").toByteArray();
}
