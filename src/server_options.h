#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <QMap>
#include <QObject>
#include <QString>

#include <common/server_id.h>

/*! Server bookmark */
class server_bookmark
{
public:
    server_id id;
    QString name;
    QString comment;
    QString password;
    QString rcon_password;
    QString ref_password;

    bool is_empty() const;
};

class server_bookmark_list : public QObject
{
    Q_OBJECT
public:
    server_bookmark_list(QObject* parent = NULL);

    typedef QMap<server_id, server_bookmark> bookmark_map_t;

    void add(const server_bookmark& bm);
    void change(const server_id& old, const server_bookmark& bm);
    void remove(const server_id& id);
    void remove_all();
    const server_bookmark& get(const server_id& id) const;

    const bookmark_map_t& list() const {return list_;}

signals:
    void changed();

private:
    bookmark_map_t list_;
};

#endif
