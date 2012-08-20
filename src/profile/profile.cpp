
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
    QVariant value;
    
    item() {}
    
    item(const QString& uid, const QString& key, const QVariant& value)
        : uid(uid), key(key), value(value) {}
    
    virtual QString sync_id() const { return uid+key; }
    
    virtual QVariantMap save() const {
        QVariantMap ret;
        ret["uid"] = uid;
        ret["key"] = key;
        ret["value"] = value;
        
        std::cerr << "SAVING:" << key.toStdString() << " value:" << value.toBool() << std::endl;
        
        return ret;
    }
    
    virtual void load(const QVariantMap& data) {
        uid = data["uid"].toString();
        key = data["key"].toString();
        value = data["value"].toString();
       
        std::cerr << "LOADING:" << key.toStdString() << " value:" << value.toBool() << std::endl;
    }
};

profile::profile()
{
}


remote::group profile::get_group()
{
    app_settings s;
    item it(app_settings::uid(), "start_hidden", s.start_hidden());
    it.set_sync_stamp(s.last_sync());
    
    std::cerr << "Preparing item"<<std::endl;
    std::cerr << "==== Profile last sync:" << it.sync_stamp().toString().toStdString() << std::endl;
    
    remote::group obj("profile");
    obj << it;
    return obj;
}


void profile::set_group(const remote::group& remote)
{
    app_settings s;
    item it;
    std::cerr << "try load..." << std::endl;
    it.load(remote.entries().begin()->save());
    std::cerr << "load ok" << std::endl;
    
    s.start_hidden_set(it.value.toBool());
    s.last_sync_set(remote.entries().begin()->sync_stamp());
}
