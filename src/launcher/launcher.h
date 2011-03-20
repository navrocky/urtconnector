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
    launcher( QObject* parent = NULL);

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
    QString launch_string(bool separate_x);
    QString launch_string();
    QString launch_string(bool use_adv_cmd_line, const QString& adv_cmd_line, 
                          const QString& binary_path, bool separate_x);
    
    /*! Game is started */
    bool is_started() const {return proc_;}

#if defined(Q_OS_UNIX)
    static QString get_separate_x_launch_str(const QString& ls);
#endif
    static void parse_combined_arg_string(const QString& launch_str, QString& program, QStringList& args);

signals:
    void started();
    void stopped();

public slots:
    void launch();
    void launch(const QString& launch_str);
    void stop();
    
private slots:
    void proc_finished(int exitCode, QProcess::ExitStatus exitStatus);
    void proc_error(QProcess::ProcessError error);

private:
    QString get_work_dir();

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
