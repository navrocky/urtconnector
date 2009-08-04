#include <QStringList>

#include "exception.h"
#include "launcher.h"

Launcher::Launcher(AppOptionsPtr opts)
    : opts_(opts)
{
    connect(&proc_, SIGNAL( finished(int, QProcess::ExitStatus) ), SLOT( procFinished(int, QProcess::ExitStatus) ));
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
    if (exitStat == QProcess::CrashExit)
        throw Exception(tr("crash"));

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
    QString cmdLine = opts_->advCmdLine()
        .replace("%bin%", opts_->binaryPath(), Qt::CaseInsensitive)
        .replace("%name%", userName_, Qt::CaseInsensitive)
        .replace("%pwd%", password_, Qt::CaseInsensitive)
        .replace("%addr%", id_.address(), Qt::CaseInsensitive)
        .replace("%rcon%", rcon_, Qt::CaseInsensitive)
        .replace("%config%", configURL_, Qt::CaseInsensitive);

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
