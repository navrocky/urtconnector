#include <QApplication>
#include <QRegExp>

#include <common/qt_syslog.h>
#include <common/str_convert.h>
#include <common/server_id.h>

#include "app_options.h"
#include "clipper.h"

SYSLOG_MODULE(clipper)

clipper::clipper( QObject* parent, app_options_p opts )
        : QObject(parent)
        , opts_( opts )
{
    connect ( QApplication::clipboard(), SIGNAL( changed(QClipboard::Mode) ), SLOT( changed(QClipboard::Mode) ) );
    connect ( QApplication::clipboard(), SIGNAL( dataChanged() ), SLOT( data_changed() ) );
}

clipper::~clipper() {}

void clipper::data_changed()
{
    changed(QClipboard::Clipboard);
}

void clipper::changed(QClipboard::Mode mode)
{
    if ( !opts_->looking_for_clip ) return;

    LOG_HARD << "Clipboard has new value";

    QRegExp rx(opts_->lfc_regexp);
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (!rx.isValid())
    {
        LOG_ERR << "Error in regexp: %1", rx.errorString();
        return;
    }

    QString clip_text = QApplication::clipboard()->text(mode);

    if (rx.indexIn(clip_text) >= 0)
    {
        QString host = rx.cap(opts_->lfc_host);
        QString port = rx.cap(opts_->lfc_port);
        QString password = rx.cap(opts_->lfc_password);

        LOG_HARD << "Match success: host=\"%1\", port=\"%2\", password=\"%3\"", host, port, password;

        QString addr;
        if (port.isEmpty())
            addr = host;
        else
            addr = host + ":" + port;

        try
        {
            server_id id(addr);
            addr = id.address();

            if (addr != address_ || password != password_)
            {
                address_ = addr;
                password_ = password;
                LOG_DEBUG << "Clipboard info obtained: %1 pass %2", address_, password_;
                emit info_obtained();
            }
        }
        catch(...)
        {
            LOG_ERR << "Syntax error in address";
        }

    } else
        LOG_HARD << "Match failed";
}
