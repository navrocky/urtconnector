#include <algorithm>

#include <boost/bind.hpp>

#include <QDir>
#include <QStringList>
#include <QSet>
#include <QTemporaryFile>

#include "tools.h"

using namespace std;
using namespace boost;

#if defined(Q_OS_UNIX)
namespace
{

QStringList find_x_locks(const QString& f)
{
    QDir dir(f);
    QStringList lst = dir.entryList(QDir::Files | QDir::Hidden | QDir::System);
    QBool(QString::*contains) (const QRegExp&) const = &QString::contains;
    lst.erase(remove_if(lst.begin(), lst.end(), !bind(contains, _1, QRegExp(".*X[0-9]+-lock.*"))), lst.end());
    return lst;
}

int extract_display(const QString& str)
{
    static QRegExp rx(".*X([0-9])+-lock.*");
    return ( rx.exactMatch(str)) ? rx.cap(1).toInt() : -1;
}

}
#endif

int find_free_display()
{
#if defined(Q_OS_UNIX)
    QSet<QString> tmpdirs;
    tmpdirs << "/tmp" << QDir::tempPath();

    QStringList locks;
    //poiner to QStringList::append method
    QStringList & (QStringList::*append) (const QStringList&) = &QStringList::operator<<;
    for_each(tmpdirs.begin(), tmpdirs.end(), bind(append, ref(locks), bind(find_x_locks, _1)));

    QList<int> locked_d;
    transform(locks.begin(), locks.end(), back_inserter(locked_d), extract_display);

    int display = 1;
    while (find(locked_d.begin(), locked_d.end(), display) != locked_d.end())
    {
        ++display;
    }
    return display;
#endif
    return 0;
}

bool try_x_start()
{
#if defined(Q_OS_UNIX)
    QTemporaryFile file;
    file.open();

    QString command = QString("xinit /bin/sh -c 'echo -n ok >> %1' -- :%2").arg(file.fileName()).arg(find_free_display());
    system(command.toStdString().c_str());

    QString readed(file.readLine());
    if (readed == "ok")
        return true;
    else
        return false;
#endif

    return false;
}
