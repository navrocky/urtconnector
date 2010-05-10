
#ifndef URT_SETTINGS_DETAIL_H
#define URT_SETTINGS_DETAIL_H

#include "settings.h"

class QSettings;

class settings_holder{
public:
    settings_holder(const QString& path, const QString& entry, settings::EntryType etype, settings::Scope);
    ~settings_holder();

    settings_holder clone() const;
    QSettings* get() const;

    const QString& path() const;
    const QString& entry() const;
    settings::EntryType etype() const;
    settings::Scope scope() const;
    const QString& filename() const;

private:
    struct HolderPimpl;
    boost::shared_ptr<HolderPimpl> p_;
};


#endif

