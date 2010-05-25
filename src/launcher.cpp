#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QProcess>

#include "exception.h"
#include "launcher.h"
#include "app_options.h"

// it's code from qprocess.cpp
static QStringList parseCombinedArgString(const QString &program)
{
    QStringList args;
    QString tmp;
    int quoteCount = 0;
    bool inQuote = false;

    // handle quoting. tokens can be surrounded by double quotes
    // "hello world". three consecutive double quotes represent
    // the quote character itself.
    for (int i = 0; i < program.size(); ++i)
    {
        if (program.at(i) == QLatin1Char('"'))
        {
            ++quoteCount;
            if (quoteCount == 3)
            {
                // third consecutive quote
                quoteCount = 0;
                tmp += program.at(i);
            }
            continue;
        }
        if (quoteCount)
        {
            if (quoteCount == 1)
                inQuote = !inQuote;
            quoteCount = 0;
        }
        if (!inQuote && program.at(i).isSpace())
        {
            if (!tmp.isEmpty())
            {
                args += tmp;
                tmp.clear();
            }
        }
        else
        {
            tmp += program.at(i);
        }
    }
    if (!tmp.isEmpty())
        args += tmp;

    return args;
}

launcher::launcher(app_options_p opts)
: opts_(opts)
{
}

QString launcher::get_work_dir()
{
    return QFileInfo(opts_->binary_path).absoluteDir().absolutePath();
}

void launcher::set_server_id(const server_id & id)
{
    id_ = id;
}

void launcher::set_user_name(const QString & value)
{
    userName_ = value;
}

void launcher::set_password(const QString & value)
{
    password_ = value;
}

void launcher::set_config_url(const QString & value)
{
    configURL_ = value;
}

void launcher::set_rcon(const QString & value)
{
    rcon_ = value;
}

void launcher::set_referee(const QString& value)
{
    referee_ = value;
}

void launcher::launch()
{
    QStringList args = parseCombinedArgString(launch_string());
    QString prog = args.first();
    args.removeFirst();

    if (!QProcess::startDetached(prog, args, get_work_dir()))
        throw qexception(tr("Failed to start UrbanTerror. Check launch parameters in options dialog."));
}

QString launcher::launch_string()
{
    QString res;
    if (opts_->use_adv_cmd_line)
    {
        res = opts_->adv_cmd_line;
        res.replace("%bin%", opts_->binary_path, Qt::CaseInsensitive)
                .replace("%name%", userName_, Qt::CaseInsensitive)
                .replace("%pwd%", password_, Qt::CaseInsensitive)
                .replace("%addr%", id_.address(), Qt::CaseInsensitive)
                .replace("%rcon%", rcon_, Qt::CaseInsensitive)
                .replace("%config%", configURL_, Qt::CaseInsensitive);
    }
    else
    {
        res = QString("\"%1\"").arg(opts_->binary_path);
        if (!userName_.isEmpty())
            res += QString(" +name \"%1\"").arg(userName_);

        if (!password_.isEmpty())
            res += QString(" +password \"%1\"").arg(password_);

        if (!rcon_.isEmpty())
            res += QString(" +rconpassword \"%1\"").arg(rcon_);

        res += QString(" +connect \"%1\"").arg(id_.address());

        res += QString(" +set fs_game q3ut4");
    }
    return res;
}
