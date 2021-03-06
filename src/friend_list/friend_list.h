#ifndef FRIEND_LIST_H
#define	FRIEND_LIST_H

#include <QObject>
#include <QMap>

#include "friend_record.h"

class friend_list : public QObject
{
    Q_OBJECT
public:
    // nick_name, friend_record
    typedef QMap<QString, friend_record> friend_records_t;

    friend_list(QObject* parent = NULL);
    
    void add(const friend_record& rec);
    void remove(const QString& nick_name);
    void remove(const QList<QString> nick_names);
    void change(const QString& old_nick_name, const friend_record& rec);

    const friend_records_t& list() const {return friends_;}
    
    friend_record get_by_nick_name(const QString& nick_name) const;
    
signals:
    void changed();
    void added(const QString& nick_name);
    void removed(const QList<QString>& nicks);
    void changed(const QString& old_nick_name, const QString& nick_name);

private:
    void add_rec(const friend_record& rec);
    
    friend class friend_list_db_saver;
    
    friend_records_t friends_;

};

#endif	/* FRIEND_LIST_H */

