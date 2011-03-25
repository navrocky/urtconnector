#ifndef FRIEND_LIST_DB_SAVER_H
#define FRIEND_LIST_DB_SAVER_H

#include <QObject>

class friend_list;

class friend_list_db_saver : public QObject
{
    Q_OBJECT
public:
    friend_list_db_saver(friend_list* list, QObject* parent);
    
private slots:
    void list_added(const QString& nick_name);
    void list_removed(const QList<QString>& nicks);
    void list_changed(const QString& old_nick_name, const QString& nick_name);
    
private:
    void remove_rec(const QString& nick_name);

    friend_list* list_;
};

#endif // FRIEND_LIST_DB_SAVER_H
