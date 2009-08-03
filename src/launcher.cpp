#include <QStringList>
#include <QProcess>

#include "launcher.h"

Launcher::Launcher(AppOptionsPtr opts)
    : opts_(opts)
{
}


Launcher::~Launcher()
{
}

void Launcher::setServerID(const ServerID & id)
{
    id_ = id;
}

void Launcher::launch()
{
    if (!opts_->useAdvCmdLine())
    {
        QStringList arguments;

        if (!userName_.isEmpty())
            arguments << QString("+name \"%1\"").arg(userName_);

        if (!password_.isEmpty())
            arguments << QString("+password \"%1\"").arg(password_);

        arguments << QString("+connect \"%1\"").arg(id_.address());

        proc_.start(opts_->binaryPath(), arguments);
        proc_.waitForFinished(100000);
    }
}

void Launcher::setUserName(const QString & value)
{
    userName_ = value;
}

void Launcher::setPassword(const QString & value)
{
    password_ = value;
}


