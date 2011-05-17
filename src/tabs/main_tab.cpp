
#include <boost/bind.hpp>

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QSettings>
#include <QShortcut>

#include <common/server_list.h>
#include <common/qaccumulatingconnection.h>
#include <common/state_settings.h>
#include <common/tools.h>

#include "main_tab.h"

namespace {
void server_to_clipboard( const server_id& id, const tab_context& ctx ){

    if( id.is_empty() )
        return;
    
    QApplication::clipboard()->setText(
        QString("/connect %1; password %2")
            .arg( id.address() )
            .arg( ctx.bookmarks()->get( id ).password() )
    );
}
}

main_tab::main_tab(const tab_settings_p& st, const tab_context& ctx, QWidget* parent)
    : QMainWindow(parent)
    , st_(st)
    , ctx_(ctx)
{
    setObjectName( st->uid() );
    setWindowFlags(windowFlags() & (~Qt::Window));

    new QAccumulatingConnection(ctx_.serv_list().get(), SIGNAL(changed()),
                                    this, SLOT(server_list_changed()),
                                    500, QAccumulatingConnection::Periodically, this);
    
    qt_signal_wrapper* qsw = new qt_signal_wrapper( this,
        boost::bind( server_to_clipboard, boost::bind( &main_tab::selected_server, this ), ctx_ ) );
    
    QAction* a = new QAction(QIcon::fromTheme("edit-copy"), tr("Copy to clipboard"), this);

    connect( new QShortcut( QKeySequence( QKeySequence::Copy ), this ), SIGNAL( activated() ), qsw, SLOT( activate() ) );
    connect( a, SIGNAL( activated() ), qsw, SLOT( activate() ) );    
    
    add_separator_action(this);
    addAction(a);
    add_separator_action(this);
}

void main_tab::load_state()
{ restoreState( st_->load_state(), 1 ); }

void main_tab::save_state()
{ st_->save_state( saveState(1) ); }

void main_tab::server_list_changed()
{ emit contents_changed(); }

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
{ st->setValue("state", a); }

QByteArray tab_settings::load_state() const
{ return st->value("state").toByteArray(); }

QByteArray tab_settings::load_header_state() const
{ return st->value("header_state").toByteArray(); }

void tab_settings::save_header_state(const QByteArray& ba)
{ st->setValue("header_state", QVariant::fromValue(ba)); }


