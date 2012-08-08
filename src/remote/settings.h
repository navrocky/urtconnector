
#ifndef URT_REMOTE_SETTINGS
#define URT_REMOTE_SETTINGS

#include <QStringList>
#include <QSet>

#include <settings/settings_generator.h>


#define MANAGER_OPTIONS \
    (( storages, QStringList, QStringList() ))

SETTINGS_GENERATE_CLASS(manager_options_base, MANAGER_OPTIONS);

class manager_options: public manager_options_base
{
public:
    manager_options(){}
    
    template <typename T>
    manager_options(const T& s) : manager_options_base(s) {}
    
    virtual QStringList storages() const
    {
        QSet<QString> s = manager_options_base::storages().toSet();
        return s.toList();
    }
    
    virtual void storages_set(const QStringList& val)
    {
        manager_options_base::storages_set(val.toSet().toList());
    }
};

// #undef MANAGER_OPTIONS;

#endif

