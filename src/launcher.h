#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QProcess>

#include "appoptions.h"
#include "serverid.h"

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class Launcher: public QObject
{
Q_OBJECT
public:
    Launcher(AppOptionsPtr opts);
    ~Launcher();

    void setServerID(const ServerID& id);
    void setUserName(const QString& value);
    void setPassword(const QString& value);
    void setRcon(const QString& value);
    void setConfigURL(const QString& value);

    /// Game is executing?
    bool executing();

    /// Launch string
    QString launchString();

public slots:
    void launch();

signals:
    void started();
    void finished();

private slots:
    void procFinished( int exitCode, QProcess::ExitStatus exitStatus );
    void procStarted();
    void procError(QProcess::ProcessError error);

private:
    void advancedLaunch();
    void simpleLaunch();
    AppOptionsPtr opts_;
    QString userName_;
    QString password_;
    ServerID id_;
    QString configURL_;
    QString rcon_;
    QProcess proc_;
    bool executing_;
};

#endif