#include <QFile>
#include <QDir>

#include <cl/syslog/syslog.h>

#include "sshot_file_output.h"

SYSLOG_MODULE(sshot_file_output)

namespace anticheat
{

sshot_file_output::sshot_file_output(QObject* parent)
: sshot_output(parent)
{
}

bool sshot_file_output::can_send_now()
{
    return true;
}

void sshot_file_output::send_file(const QString& name, const QByteArray& data,
                                  bool)
{
    if (!is_enabled())
        return;
    QString fn;

    if (folder_.isEmpty())
        fn = name;
    else
    {
        QDir d;
        if (!d.exists(folder_))
        {
            if (!d.mkpath(folder_))
                LOG_ERR << "Can't create folder \"%1\"", folder_.toLocal8Bit().data();
        }
        fn = QString("%1/%2").arg(folder_).arg(name);
    }

    QFile f(fn);
    if (!f.open(QIODevice::WriteOnly))
    {
        LOG_ERR << "Can't open file to write \"%1\"", fn.toLocal8Bit().data();
        return;
    }
    if (f.write(data) < 0)
    {
        LOG_ERR << "Error while writing file \"%1\"", fn.toLocal8Bit().data();
        return;
    }
}

void sshot_file_output::set_folder(const QString& val)
{
    folder_ = val;
}

}
