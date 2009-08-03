#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QProcess>

#include "appoptions.h"
#include "serverid.h"

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class Launcher
{
public:
    Launcher(AppOptionsPtr opts);
    ~Launcher();

    void setServerID(const ServerID& id);
    void setUserName(const QString& value);
    void setPassword(const QString& value);

    void launch();

private:
    AppOptionsPtr opts_;
    QString userName_;
    QString password_;
    ServerID id_;
    QProcess proc_;
};

#endif
