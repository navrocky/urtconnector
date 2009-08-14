#ifndef SERVEROPTIONS_H
#define SERVEROPTIONS_H

#include <map>

#include <QString>
#include <QUuid>

#include "serverid.h"

class ServerOptions
{
public:
    ServerOptions();
    ServerOptions(const ServerOptions& src);
    ~ServerOptions();

    ServerOptions& operator= (const ServerOptions& src);

    const QUuid& uid() const {return uid_;}
    void setUid(const QUuid& val);

    const ServerID& id() const {return id_;}
    void setId(const ServerID& val);

    QString name() const {return name_;}
    void setName(const QString& val);

    QString comment() const {return comment_;}
    void setComment(const QString& val);

    bool favorite() const {return favorite_;}
    void setFavorite(bool val);

    QString rconPassword() const {return rconPassword_;}
    void setRconPassword(const QString& val);

    QString refPassword() const {return refPassword_;}
    void setRefPassword(const QString& val);

    QString folder() const {return folder_;}
    void setFolder(const QString& val);

private:
    void assign(const ServerOptions& src);

    QUuid uid_;
    ServerID id_;
    QString name_;
    QString comment_;
    bool favorite_;
    QString rconPassword_;
    QString refPassword_;
    QString folder_;
};


/// List of ServerOptions
typedef std::map<QUuid, ServerOptions> ServerOptionsList;

#endif
