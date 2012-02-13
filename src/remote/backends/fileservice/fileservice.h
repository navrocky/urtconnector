
#ifndef URT_FILESERVICE_H
#define URT_FILESERVICE_H

#include "../../storage.h"
#include "../../remote.h"

class filestorage: public QObject, public remote::storage {
Q_OBJECT
public:
    
    explicit filestorage(const QString& location, QObject* parent = 0);
    virtual ~filestorage();

    virtual remote::action* get(const QString& type);
    virtual remote::action* put(const remote::group& gr);
    virtual remote::action* check(const QString& type);
  
private:
    QString location_;
};

struct fileservice: public remote::service {

    fileservice()
        : service("fileservice", "Synchronization with files")
    {}

    remote::service::Storage do_create(const boost::shared_ptr<QSettings>& settings) const;
    virtual QVariantMap configure(const QVariantMap& settings) const;
};


#endif
