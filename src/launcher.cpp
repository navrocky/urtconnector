#include <QStringList>
#include <QDir>
#include <QFileInfo>

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

void launcher::procFinished(int, QProcess::ExitStatus exitStat)
{
    executing_ = false;
    emit finished();
    if (exitStat == QProcess::CrashExit)
        throw qexception(tr("Game crashed"));
}

void launcher::procError(QProcess::ProcessError)
{
    if (executing_)
    {
        executing_ = false;
        emit finished();
    }
    throw qexception(tr("Launch error. Check launch parameters."));
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
    proc_.setWorkingDirectory(get_work_dir());
    proc_.start(launch_string());
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
    if (opts_->use_adv_cmd_line)
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

        if (!rcon_.isEmpty())
            res += QString(" +rconpassword \"%1\"").arg(rcon_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString(" +set fs_game q3ut4");
    }
    return res;
}
