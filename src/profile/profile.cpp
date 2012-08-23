
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/foreach.hpp>

#include <QSettings>


#include <settings/settings.h>
#include "app_options.h"
#include "anticheat/settings.h"

#include "remote/remote.h"

#include "profile.h"


struct item : public remote::syncable
{
    QString uid;
    QString key;
    
    item() {}
    
    item(const QString& uid, const QString& key)
        : uid(uid), key(key)
    {
        set_sync_stamp(lastmod());
    }
    
    virtual QString sync_id() const { return uid+key; }
    
    virtual QVariantMap save() const {
        QVariantMap ret;
        ret["uid"] = uid;
        ret["key"] = key;
        ret["value"] = base_settings().get_settings(uid)->value(key);
        
        return ret;
    }
    
    virtual void load(const QVariantMap& data) {
        uid = data["uid"].toString();
        key = data["key"].toString();
        const QVariant value = data["value"];
         set_sync_stamp(data[remote::syncable::stamp_key].toDateTime());
        base_settings().get_settings(uid)->setValue(key, value);
        
        touch();
    }
    
    inline QDateTime lastmod() const 
    {
        return base_settings().get_settings("profile")->value("lastmod_" + key
            , QDateTime::currentDateTime().toString(Qt::ISODate)).toDateTime();
    }
    
    inline void touch() 
    {
        base_settings().get_settings("profile")->setValue("lastmod_" + key, sync_stamp().toString(Qt::ISODate));
    }
};

std::list<std::pair<QString, QString> > make_list()
{
    std::list<std::pair<QString, QString> > ret;
    ret.push_back(std::make_pair(app_settings::uid(), "start_hidden"));
    return ret;
}

const std::list<std::pair<QString, QString> > item_desc = make_list();


profile::profile()
{
}


remote::group profile::get_group()
{
    remote::group obj("profile");
    
    std::pair<QString, QString> p;
    BOOST_FOREACH(p, item_desc)
    {
        item it = item(p.first, p.second);
        obj << it;
        std::cerr << "Preparing item"<<std::endl;
        std::cerr << "==== Profile last sync:" << it.sync_stamp().toString().toStdString() << std::endl;        
    }

    return obj;
}

void profile::set_group(const remote::group& remote)
{
    BOOST_FOREACH(remote::group::Entries::value_type i, remote.entries())
    {
        std::cerr << "SETTING ITEM"<<std::endl;
        item it;
        it.load(i.save());
    }
}
