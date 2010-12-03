#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>
#include <QProcess>
#include <QPointer>

#include "../pointers.h"
#include <common/server_id.h>

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class launcher: public QObject
{
    Q_OBJECT
public:
    launcher(app_options_p launch, QObject* parent = NULL);

    server_id id() const {return id_;}
    void set_server_id(const server_id& id);

    QString user_name() const {return user_name_;}
    void set_user_name(const QString& value);

    QString password() const {return password_;}
    void set_password(const QString& value);

    void set_rcon(const QString& value);
    void set_referee(const QString& value);
    void set_config_url(const QString& value);
    
    /*! Urban Terror detaches from UrTconnector  */
    bool is_detach() const {return detach_;}
    void set_detach(bool val);

    /*! Launch string calculated from options above */
    QString launch_string();

    /*! Game is started */
    bool is_started() const {return proc_;}

signals:
    void started();
    void stopped();

public slots:
    void launch();
    void stop();
    
private slots:
    void proc_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void proc_error(QProcess::ProcessError error);

private:
    QString get_work_dir();

    app_options_p opts_;
    QString user_name_;
    QString password_;
    server_id id_;
    QString config_url_;
    QString rcon_;
    QString referee_;
    bool detach_;
    QPointer<QProcess> proc_;
};

#endif
