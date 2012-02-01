#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <QMap>
#include <QObject>
#include <QString>

#include "server_id.h"
#include "implicit_sharing.h"

#include "remote/remote.h"

/*! Server bookmark */
class server_bookmark: public remote::syncable
{
public:
    server_bookmark(){};
    
    server_bookmark(const server_id& id,
                    const QString& name,
                    const QString& comment,
                    const QString& password,
                    const QString& rcon_password,
                    const QString& ref_password,
                    const QDateTime& sync_stamp);

    const server_id& id() const { return d->id; }
    void set_id(const server_id& val) { d->id = val; set_sync_stamp(); }

    const QString& name() const { return d->name; }
    void set_name(const QString& val) { d->name = val; set_sync_stamp(); }

    const QString& comment() const { return d->comment; }
    void set_comment(const QString& val) { d->comment = val; set_sync_stamp(); }

    const QString& password() const { return d->password; }
    void set_password(const QString& val) { d->password = val; set_sync_stamp(); }

    const QString& rcon_password() const { return d->rcon_password; }
    void set_rcon_password(const QString& val) { d->rcon_password = val; set_sync_stamp(); }

    const QString& ref_password() const { return d->ref_password; }
    void set_ref_password(const QString& val) { d->ref_password = val; set_sync_stamp(); }

    bool is_empty() const { return d->id.is_empty(); }

    static const server_bookmark& empty();
    
    virtual QString sync_id() const { return id().address(); };
    
    /*! serialize object*/
    virtual QVariantMap save() const{
        QVariantMap m;
        m["ip"] = id().ip();
        m["hostname"] = id().host_name();
        m["port"] = id().port();
        m["name"] = name();
        m["comment"] = comment();
        m["password"] = password();
        m["rcon_password"] = rcon_password();
        m["ref_password"] = ref_password();
        return m;
    };
    /*! deserialize object*/
    virtual void load(const QVariantMap& data){
        set_id( server_id(data["ip"].toString(), data["hostname"].toString(), data["port"].toInt()) );
        set_name( data["name"].toString() );
        set_comment( data["comment"].toString() );
        set_password( data["password"].toString() );
        set_rcon_password( data["rcon_password"].toString() );
        set_ref_password( data["ref_password"].toString() );
        set_sync_stamp( data[remote::syncable::stamp_key].toDateTime() );
    }

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

    typedef QMap<server_id, server_bookmark> bookmark_map_t;

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
