#include <QStringList>

#include "exception.h"
#include "launcher.h"

launcher::launcher(app_options_ptr opts)
    : opts_(opts),
      executing_(false)
{
    connect(&proc_, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(procFinished(int,QProcess::ExitStatus)));
    connect(&proc_, SIGNAL(started()), SLOT(procStarted()));
    connect(&proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(procError(QProcess::ProcessError)));
}

launcher::~launcher()
{
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

void launcher::procFinished(int, QProcess::ExitStatus exitStat)
{
    executing_ = false;
    if (exitStat == QProcess::CrashExit)
        throw qexception(tr("Game crashed"));

    emit finished();
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
    if (!opts_->use_adv_cmd_line)
        simpleLaunch();
    else
        advancedLaunch();
}

void launcher::advancedLaunch()
{
    QString cmdLine = launch_string();

#ifdef Q_WS_X11
    QStringList args;
    args << "-c" << cmdLine;
    proc_.start("/bin/sh", args);
#else
    // TODO needed to write advanced launch for windows and mac


#endif
}

void launcher::simpleLaunch()
{
    QStringList arguments;

    if (!userName_.isEmpty())
        arguments << QString("+name \"%1\"").arg(userName_);

    if (!password_.isEmpty())
        arguments << QString("+password \"%1\"").arg(password_);

    if (!rcon_.isEmpty())
        arguments << QString("+rconpassword \"%1\"").arg(rcon_);

    arguments << QString("+connect \"%1\"").arg(id_.address());

    arguments << QString("+set fs_game q3ut4");

    proc_.start(opts_->binary_path, arguments);
}

void launcher::procStarted()
{
    executing_ = true;
    emit started();
}

bool launcher::executing()
{
    return executing_;
}

QString launcher::launch_string()
{
    QString res;
    if (!opts_->use_adv_cmd_line)
    {
        res = opts_->adv_cmd_line
            .replace("%bin%", opts_->binary_path, Qt::CaseInsensitive)
            .replace("%name%", userName_, Qt::CaseInsensitive)
            .replace("%pwd%", password_, Qt::CaseInsensitive)
            .replace("%addr%", id_.address(), Qt::CaseInsensitive)
            .replace("%rcon%", rcon_, Qt::CaseInsensitive)
            .replace("%config%", configURL_, Qt::CaseInsensitive);
    } else
    {
        res = opts_->binary_path;
        if (!userName_.isEmpty())
            res += QString(" +name \"%1\"").arg(userName_);

        if (!password_.isEmpty())
            res += QString(" +password \"%1\"").arg(password_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString("+set fs_game q3ut4");
    }
    return res;
}

void launcher::procError(QProcess::ProcessError)
{
    throw qexception(tr("Launch error. Check launch parameters."));
}
