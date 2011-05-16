#include <QApplication>
#include <QRegExp>

#include <common/qt_syslog.h>
#include <common/str_convert.h>
#include <common/server_id.h>

#include "app_options.h"
#include "clipper.h"

SYSLOG_MODULE(clipper)

clipper::clipper( QObject* parent )
        : QObject(parent)
{
    connect ( QApplication::clipboard(), SIGNAL( changed(QClipboard::Mode) ), SLOT( changed(QClipboard::Mode) ) );
}

clipper::~clipper() {}

void clipper::changed(QClipboard::Mode mode)
{
    clip_settings cs;
    if ( !cs.watching() ) return;

    LOG_HARD << "Clipboard has new value";

    QRegExp rx(cs.regexp());
    rx.setCaseSensitivity(Qt::CaseInsensitive);
    if (!rx.isValid())
    {
        LOG_ERR << "Error in regexp: %1", rx.errorString();
        return;
    }

    QString clip_text = QApplication::clipboard()->text(mode);

    if (rx.indexIn(clip_text) >= 0)
    {
        QString host = rx.cap(cs.host());
        QString port = rx.cap(cs.port());
        QString password = rx.cap(cs.password());

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
