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
static QStringList __parse_combined_arg_string(const QString &program)
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

launcher::launcher(QObject* parent)
: QObject(parent)
, detach_(false)
, mumble_overlay_(false)
{
}

void launcher::parse_combined_arg_string(const QString& launch_str, QString& program, QStringList& args)
{
    args = __parse_combined_arg_string(launch_str);
    program = args.first();
    args.removeFirst();
}

QString launcher::get_work_dir()
{
    return QFileInfo(app_settings().binary_path()).absoluteDir().absolutePath();
}

void launcher::set_server_id(const server_id & id)
{
    id_ = id;
}

void launcher::set_user_name(const QString & value)
{
    user_name_ = value;
}

void launcher::set_password(const QString & value)
{
    password_ = value;
}

void launcher::set_config_url(const QString & value)
{
    config_url_ = value;
}

void launcher::set_mumble_overlay(bool value)
{
    mumble_overlay_ = value;
}

void launcher::set_mumble_overlay_bin(const QString& value)
{
    mumble_overlay_bin_ = value;
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
    launch(launch_string());
}

void launcher::launch(const QString& ls)
{
    // prepare launch parameters
    QStringList args;
    QString prog;
    parse_combined_arg_string(ls, prog, args);

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

QString launcher::launch_string(bool separate_x)
{
    app_settings as;
    return launch_string(as.use_adv_cmd_line(), as.adv_cmd_line(),
                         as.binary_path(), separate_x);
}


QString launcher::launch_string ( bool use_adv_cmd_line,
                                  const QString& adv_cmd_line,
                                  const QString& binary_path,
                                  bool separate_x )
{
    QString res;
    if ( use_adv_cmd_line )
    {
        res = adv_cmd_line;
        res.replace("%bin%", binary_path, Qt::CaseInsensitive)
                .replace("%name%", user_name_, Qt::CaseInsensitive)
                .replace("%pwd%", password_, Qt::CaseInsensitive)
                .replace("%addr%", id_.address(), Qt::CaseInsensitive)
                .replace("%rcon%", rcon_, Qt::CaseInsensitive)
                .replace("%config%", config_url_, Qt::CaseInsensitive);
    }
    else
    {
        res = QString("\"%1\"").arg( binary_path );
        if (!user_name_.isEmpty())
            res += QString(" +name \"%1\"").arg(user_name_);

        if (!password_.isEmpty())
            res += QString(" +password \"%1\"").arg(password_);

        if (!rcon_.isEmpty())
            res += QString(" +rconpassword \"%1\"").arg(rcon_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString(" +set fs_game q3ut4");
    }

#if defined(Q_OS_UNIX)
    if (mumble_overlay_)
        res = mumble_overlay_bin_ + " " + res;

    if ( separate_x )
        res = get_separate_x_launch_str(res);
#endif

    return res;
}

#if defined(Q_OS_UNIX)
QString launcher::get_separate_x_launch_str(const QString& ls)
{
    return QString("xinit %1 -- :%2").arg(ls).arg( find_free_display() );
}
#endif

QString launcher::launch_string()
{
    return launch_string( app_settings().separate_xsession() );
}

void launcher::stop()
{
    if (!proc_)
        return;
    delete proc_;
}

void launcher::set_detach(bool val)
{
    detach_ = val;
}
