#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <QMap>
#include <QObject>
#include <QString>

#include <common/server_id.h>
#include <common/implicit_sharing.h>

/*! Server bookmark */
class server_bookmark
{
public:
    server_bookmark(){};
    
    server_bookmark(const server_id& id,
                    const QString& name,
                    const QString& comment,
                    const QString& password,
                    const QString& rcon_password,
                    const QString& ref_password);

    const server_id& id() const { return d->id; }
    void set_id(const server_id& val) { d->id = val; }

    const QString& name() const { return d->name; }
    void set_name(const QString& val) { d->name = val; }

    const QString& comment() const { return d->comment; }
    void set_comment(const QString& val) { d->comment = val; }

    const QString& password() const { return d->password; }
    void set_password(const QString& val) { d->password = val; }

    const QString& rcon_password() const { return d->rcon_password; }
    void set_rcon_password(const QString& val) { d->rcon_password = val; }

    const QString& ref_password() const { return d->ref_password; }
    void set_ref_password(const QString& val) { d->ref_password = val; }

    bool is_empty() const { return d->id.is_empty(); }

private:
    struct impl
    {
        server_id id;
        QString name;
        QString comment;
        QString password;
        QString rcon_password;
        QString ref_password;
    };
    implicit_sharing<impl> d;
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
