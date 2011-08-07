#include "db_saver.h"

#include <memory>
#include <exception>
#include <common/qt_syslog.h>
#include <database/sqlite_database.h>

#include "manager.h"
#include "task.h"
#include "common.h"

SYSLOG_MODULE(db_saver)

namespace tracking
{

db_saver::db_saver(manager* list, const condition_factory_p& conds,
             const action_factory_p& acts, QObject* parent)
: QObject(parent)
, list_(list)
{
    // initialize database
    try
    {
        database* db = database::instance();
        db->query
        (
            "CREATE TABLE IF NOT EXISTS tracking_tasks"
            "("
            "    id VARCHAR(255),"
            "    body TEXT,"
            "    PRIMARY KEY(id)"
            ");"
        );
        
        // load list
        database::result_set res = database::instance()->query("SELECT * FROM tracking_tasks;");
        foreach (const database::result_row& row, res)
        {
            std::auto_ptr<task_t> task(new task_t(list_));

            QString body = row[1];
            QByteArray ba = QByteArray::fromBase64(body.toAscii());
            QDataStream ds(ba);
            settings_t s;
            ds >> s;
            task->load(s, conds, acts);
            list_->add_task(task.get());
            task.release();
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERR << e.what();
    }
    
    connect(list_, SIGNAL(task_added(task_t*)), SLOT(task_added(task_t*)));
    connect(list_, SIGNAL(task_changed(task_t*)), SLOT(task_changed(task_t*)));
    connect(list_, SIGNAL(task_removed(QString)), SLOT(task_removed(QString)));
}

void db_saver::task_added(task_t* t)
{
    task_changed(t);
}

void db_saver::task_changed(task_t* t)
{
    // removing old record if any
    task_removed(t->id());
    
    // inserting new record
    settings_t s;
    t->save(s);
    QByteArray ba;
    {
        QDataStream ds(&ba, QIODevice::WriteOnly);
        ds << s;
    }
    QString body = ba.toBase64();

    database* db = database::instance();
    db->query(QString("INSERT INTO tracking_tasks VALUES('%1', '%2');")
        .arg(t->id())
        .arg(body)
        );
}

void db_saver::task_removed(const QString& id)
{
    database* db = database::instance();
    db->query(QString("DELETE FROM tracking_tasks WHERE id='%1';").arg(id));
}

}

