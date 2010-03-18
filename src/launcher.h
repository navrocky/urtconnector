#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QProcess>

#include "appoptions.h"
#include "serverid.h"

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class launcher: public QObject
{
Q_OBJECT
public:
    launcher(app_options_ptr opts);
    ~launcher();

    void set_server_id(const server_id& id);
    void set_user_name(const QString& value);
    void set_password(const QString& value);
    void set_rcon(const QString& value);
    void set_referee(const QString& value);
    void set_config_url(const QString& value);

    // Game is executing?
    bool executing();

    // Launch string
    QString launch_string();

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
    QString get_work_dir();
    app_options_ptr opts_;
    QString userName_;
    QString password_;
    server_id id_;
    QString configURL_;
    QString rcon_;
    QString referee_;
    QProcess proc_;
    bool executing_;
};

#endif
