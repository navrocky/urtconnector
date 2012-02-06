#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <QMap>
#include <QObject>
#include <QString>

#include "server_id.h"
#include "implicit_sharing.h"

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

    bool is_empty() const { return d->id.is_empty();}

    static const server_bookmark& empty();

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
    server_bookmark_list();

    typedef std::map<server_id, server_bookmark> bookmark_map_t;

    ///Add bookmark \p bm to list
    void add( const server_bookmark& bm );
    
    ///Change bookmark \p bm if it's server_id changed
    void change( const server_id& old, const server_bookmark& bm );
    
    ///Change bookmark \p bm 
    void change( const server_bookmark& bm );
    
    ///remove bookmark stored by \p id 
    void remove( const server_id& id );
    
    ///remove bookmark
    void remove( const server_bookmark& bm );
    
    void clear();
    
    const server_bookmark& get(const server_id& id) const;

    const bookmark_map_t& list() const { return list_; }
    
    int size() const { return list_.size(); }

signals:
    
    /*!
     * @brief this signal emited every time when some changes makes with bookmark 
     *
     * @param old_bm previous bookmark
     * @param new_bm current bookmark
     * @note when bookmark added \b old_bn.is_empty() returns true. when bookmark is deleted \p new_bn.is_empty() returns true;
     **/
    
    void changed( const server_bookmark& old_bm, const server_bookmark& new_bm );
    
private:
    bookmark_map_t list_;
};

typedef boost::shared_ptr<server_bookmark_list> server_bookmark_list_p;

#endif
