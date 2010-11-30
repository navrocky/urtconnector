#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include <common/qt_syslog.h>
#include <common/exception.h>
#include "../app_options.h"

#include "tools.h"
#include "launcher.h"

SYSLOG_MODULE(launcher)

namespace
{

// it's code from qprocess.cpp
static QStringList parse_combined_arg_string(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i)
    {
        if (program.at(i) == QLatin1Char('"'))
        {
            ++quoteCount;
            if (quoteCount == 3)
            {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount)
        {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace())
        {
            if (!tmp.isEmpty())
            {
                args += tmp;
                tmp.clear();
            }
        }
        else
        {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

}

////////////////////////////////////////////////////////////////////////////////
// launcher

launcher::launcher(app_options_p opts, QObject* parent)
: QObject(parent)
, opts_(opts)
, detach_(false)
{
}

QString launcher::get_work_dir()
{
    return QFileInfo(opts_->binary_path).absoluteDir().absolutePath();
}

void launcher::set_server_id(const server_id & id)
{
    id_ = id;
}

void launcher::set_user_name(const QString & value)
{
    userName_ = value;
}

void launcher::set_password(const QString & value)
{
    password_ = value;
}

void launcher::set_config_url(const QString & value)
{
    configURL_ = value;
}

void launcher::set_rcon(const QString & value)
{
    rcon_ = value;
}

void launcher::set_referee(const QString& value)
{
    referee_ = value;
}

void launcher::launch()
{
    // prepare launch parameters
    QString ls = launch_string();
    QStringList args = parse_combined_arg_string(ls);
    QString prog = args.first();
    args.removeFirst();

    if (detach_)
    {
        LOG_DEBUG << "Detached game launch: \"%1\"", ls;
        if (!QProcess::startDetached(prog, args, get_work_dir()))
            throw qexception(tr("Failed to start UrbanTerror. Check launch parameters in options dialog."));
    } else
    {
        if (proc_)
            throw qexception(tr("Game already started. Close it if you want and try again."));

        LOG_DEBUG << "Game launch: \"%1\"", ls;
        proc_ = new QProcess(this);
        connect(proc_, SIGNAL(finished(int, QProcess::ExitStatus)), SLOT(proc_finished(int, QProcess::ExitStatus)));
        connect(proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(proc_error(QProcess::ProcessError)));
        connect(proc_, SIGNAL(started()), SIGNAL(started()));
        proc_->setWorkingDirectory(get_work_dir());
        proc_->start(prog, args);
    }
}

void launcher::proc_finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    LOG_DEBUG << "Game finished";
    delete proc_;
    emit stopped();
}

void launcher::proc_error(QProcess::ProcessError error)
{
    delete proc_;
    emit stopped();
    throw qexception(tr("Game launch error \"%1\".").arg(error));
}

QString launcher::launch_string()
{
    QString res;
    if (opts_->use_adv_cmd_line)
    {
        res = opts_->adv_cmd_line;
        res.replace("%bin%", opts_->binary_path, Qt::CaseInsensitive)
                .replace("%name%", userName_, Qt::CaseInsensitive)
                .replace("%pwd%", password_, Qt::CaseInsensitive)
                .replace("%addr%", id_.address(), Qt::CaseInsensitive)
                .replace("%rcon%", rcon_, Qt::CaseInsensitive)
                .replace("%config%", configURL_, Qt::CaseInsensitive);
    }
    else
    {
        res = QString("\"%1\"").arg(opts_->binary_path);
        if (!userName_.isEmpty())
            res += QString(" +name \"%1\"").arg(userName_);

        if (!password_.isEmpty())
            res += QString(" +password \"%1\"").arg(password_);

        if (!rcon_.isEmpty())
            res += QString(" +rconpassword \"%1\"").arg(rcon_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString(" +set fs_game q3ut4");
    }

#if defined(Q_OS_UNIX)
    if ( opts_->separate_x )
        res = QString("xinit %1 -- :%2").arg(res).arg( find_free_display() );
#endif
    
    return res;
}
