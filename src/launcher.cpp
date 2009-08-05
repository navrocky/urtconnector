#include <QStringList>

#include "exception.h"
#include "launcher.h"

Launcher::Launcher(AppOptionsPtr opts)
    : opts_(opts),
      executing_(false)
{
    connect(&proc_, SIGNAL(finished(int,QProcess::ExitStatus)), SLOT(procFinished(int,QProcess::ExitStatus)));
    connect(&proc_, SIGNAL(started()), SLOT(procStarted()));
    connect(&proc_, SIGNAL(error(QProcess::ProcessError)), SLOT(procError(QProcess::ProcessError)));
}

Launcher::~Launcher()
{
}

void Launcher::setServerID(const ServerID & id)
{
    id_ = id;
}

void Launcher::setUserName(const QString & value)
{
    userName_ = value;
}

void Launcher::setPassword(const QString & value)
{
    password_ = value;
}

void Launcher::procFinished(int, QProcess::ExitStatus exitStat)
{
    executing_ = false;
    if (exitStat == QProcess::CrashExit)
        throw Exception(tr("Game crashed"));

    emit finished();
}

void Launcher::setConfigURL(const QString & value)
{
    configURL_ = value;
}

void Launcher::setRcon(const QString & value)
{
    rcon_ = value;
}

void Launcher::launch()
{
    if (!opts_->useAdvCmdLine())
        simpleLaunch();
    else
        advancedLaunch();
}

void Launcher::advancedLaunch()
{
    QString cmdLine = launchString();

#ifdef Q_WS_X11
    QStringList args;
    args << "-c" << cmdLine;
    proc_.start("/bin/sh", args);
#else
    // TODO needed to write advanced launch for windows and mac


#endif
}

void Launcher::simpleLaunch()
{
    QStringList arguments;

    if (!userName_.isEmpty())
        arguments << QString("+name \"%1\"").arg(userName_);

    if (!password_.isEmpty())
        arguments << QString("+password \"%1\"").arg(password_);

    arguments << QString("+connect \"%1\"").arg(id_.address());

    arguments << QString("+set fs_game q3ut4");

    proc_.start(opts_->binaryPath(), arguments);
}

void Launcher::procStarted()
{
    executing_ = true;
    emit started();
}

bool Launcher::executing()
{
    return executing_;
}

QString Launcher::launchString()
{
    QString res;
    if (!opts_->useAdvCmdLine())
    {
        res = opts_->advCmdLine()
            .replace("%bin%", opts_->binaryPath(), Qt::CaseInsensitive)
            .replace("%name%", userName_, Qt::CaseInsensitive)
            .replace("%pwd%", password_, Qt::CaseInsensitive)
            .replace("%addr%", id_.address(), Qt::CaseInsensitive)
            .replace("%rcon%", rcon_, Qt::CaseInsensitive)
            .replace("%config%", configURL_, Qt::CaseInsensitive);
    } else
    {
        res = opts_->binaryPath();
        if (!userName_.isEmpty())
            res += QString(" +name \"%1\"").arg(userName_);

        if (!password_.isEmpty())
            res += QString(" +password \"%1\"").arg(password_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString("+set fs_game q3ut4");
    }
    return res;
}

void Launcher::procError(QProcess::ProcessError)
{
    throw Exception(tr("Launch error. Check launch parameters."));
}
