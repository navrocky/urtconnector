
#ifndef URT_FILESERVICE_H
#define URT_FILESERVICE_H

#include "../../storage.h"
#include "../../remote.h"

class filestorage: public QObject, public remote::storage {
Q_OBJECT
public:
    
    explicit filestorage(const QString& path, QObject* parent = 0);
    virtual ~filestorage();

    virtual remote::action* get(const QString& type);
    virtual remote::action* put(const remote::group& gr);
    virtual remote::action* check(const QString& type);
  
private:
    QString path_;
};


#endif
