#include <QFile>

#include <cl/syslog/syslog.h>

#include "sshot_file_output.h"

SYSLOG_MODULE("sshot_file_output")

sshot_file_output::sshot_file_output(QObject* parent)
: sshot_output(parent)
{
}

bool sshot_file_output::can_send_now()
{
    return true;
}

void sshot_file_output::send_file(const QString& name, const QByteArray& data)
{
    if (!is_enabled())
        return;

    QFile f(name);
    if (!f.open(QIODevice::WriteOnly))
    {
        LOG_ERR << "Can't open file to write \"%1\"", name.toLocal8Bit().data();
        return;
    }
    if (f.write(data) < 0)
    {
        LOG_ERR << "Error while writing file \"%1\"", name.toLocal8Bit().data();
        return;
    }
}

