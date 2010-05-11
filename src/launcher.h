#ifndef LAUNCHER_H
#define LAUNCHER_H

#include <QObject>

#include "settings/settings.h"
#include "pointers.h"
#include "server_id.h"

/*! \brief Game launcher.

    Take options, username, server address and launch game.
*/

class launcher: public QObject
{
Q_OBJECT
public:
    launcher(const settings& launch);

    void set_server_id(const server_id& id);
    void set_user_name(const QString& value);
    void set_password(const QString& value);
    void set_rcon(const QString& value);
    void set_referee(const QString& value);
    void set_config_url(const QString& value);

    // Launch string
    QString launch_string();

public slots:
    void launch();

private:
    QString get_work_dir();
    settings launch_opts;
    QString userName_;
    QString password_;
    server_id id_;
    QString configURL_;
    QString rcon_;
    QString referee_;
};

#endif
