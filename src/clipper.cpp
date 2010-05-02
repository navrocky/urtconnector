
#include <QApplication>

#include "app_options.h"
#include "server_id.h"
#include "clipper.h"


clipper::clipper( QObject* parent, app_options_p opts )
    : QObject(parent)
    , opts_( opts )
{
    connect ( QApplication::clipboard(), SIGNAL( changed(QClipboard::Mode) ), SLOT( changed(QClipboard::Mode) ) );
}

clipper::~clipper(){}

void clipper::changed(QClipboard::Mode mode)
{
    if ( !opts_->looking_for_clip ) return;
    
    try {
        server_id id( QApplication::clipboard()->text(mode) );
        if ( id.address() != addr_ ) addr_ = id.address();
        emit address_obtained( addr_ );
    }
    catch(...)
    {}
}