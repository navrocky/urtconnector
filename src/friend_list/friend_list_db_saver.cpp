#include "friend_list_db_saver.h"

#include <exception>
#include <common/qt_syslog.h>
#include <database/sqlite_database.h>
#include "friend_list.h"

SYSLOG_MODULE(friend_list_db_saver)

friend_list_db_saver::friend_list_db_saver(friend_list* list, QObject* parent)
: QObject(parent)
, list_(list)
{
    // initialize database
    try
    {
        database* db = database::instance();
        db->query
        (
            "CREATE TABLE IF NOT EXISTS friends"
            "("
            "    nick_name VARCHAR(255),"
            "    expression VARCHAR(255),"
            "    comment TEXT,"
            "    PRIMARY KEY(nick_name)"
            ");"
        );
        
        // load list
        database::result_set res = database::instance()->query("SELECT * FROM friends;");
        foreach (const database::result_row& row, res)
        {
            friend_record fr;
            fr.set_nick_name(DB_DEC(row[0]));
            fr.set_expression(DB_DEC(row[1]));
            fr.set_comment(DB_DEC(row[2]));
            list_->add_rec(fr);
        }
    }
    catch(const std::exception& e)
    {
        LOG_ERR << e.what();
    }
    
    connect(list, SIGNAL(added(QString)), SLOT(list_added(QString)));
    connect(list, SIGNAL(changed(QString,QString)), SLOT(list_changed(QString,QString)));
    connect(list, SIGNAL(removed(QList<QString>)), SLOT(list_removed(QList<QString>)));
}

void friend_list_db_saver::remove_rec(const QString& nick_name)
{
    database* db = database::instance();
    db->query(QString("DELETE FROM friends WHERE nick_name='%1';")
        .arg(DB_ENC(nick_name)));
}


void friend_list_db_saver::list_added(const QString& nick_name)
{
    list_changed(nick_name, nick_name);
}

void friend_list_db_saver::list_changed(const QString& old_nick_name, const QString& nick_name)
{
    const friend_record& fr = list_->get_by_nick_name(nick_name);
    
    // removing old record if any
    remove_rec(old_nick_name);
    
    // inserting new record
    database* db = database::instance();
    db->query(QString("INSERT INTO friends VALUES('%1', '%2', '%3');")
        .arg(DB_ENC(fr.nick_name()))
        .arg(DB_ENC(fr.expression()))
        .arg(DB_ENC(fr.comment()))
        );
}

void friend_list_db_saver::list_removed(const QList< QString >& nicks)
{
    foreach (const QString& nn, nicks)
    {
        remove_rec(nn);
    }
}

