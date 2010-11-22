#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QProcess>
#include <QPointer>

#include "../pointers.h"
#include <common/server_id.h>
#include <anticheat/manager.h>

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class launcher: public QObject
{
    Q_OBJECT
public:
    launcher(app_options_p launch, anticheat::manager* anticheat, QObject* parent = NULL);

    void set_server_id(const server_id& id);
    void set_user_name(const QString& value);
    void set_password(const QString& value);
    void set_rcon(const QString& value);
    void set_referee(const QString& value);
    void set_config_url(const QString& value);

    server_id id() const
    {
        return id_;
    }
    QString userName() const
    {
        return userName_;
    }
    QString password() const
    {
        return password_;
    }

    // Launch string
    QString launch_string();

public slots:
    void launch();
    
private slots:
    void proc_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void proc_error(QProcess::ProcessError error);

private:
    QString get_work_dir();
    app_options_p opts_;
    QString userName_;
    QString password_;
    server_id id_;
    QString configURL_;
    QString rcon_;
    QString referee_;
    anticheat::manager* anticheat_;
    QPointer<QProcess> proc_;
};

#endif
