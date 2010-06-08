

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "settings.h"

const QSettings::Format format_c = QSettings::IniFormat;

struct settings::Pimpl{

    typedef std::map<QString, settings_ptr > settings_map;
    Pimpl()
        : settings( new QSettings( QSettings::IniFormat, QSettings::UserScope,
            QCoreApplication::organizationName(), QCoreApplication::applicationName() ) )
    {}

    ///Get dir to the main QSettings object
    QString dir_path() const
    {
        return QFileInfo( settings->fileName() ).absoluteDir().path() + "/";
    }

    ///Get internal QSettings by \p uid. Group is created if no group found.
    settings_ptr get_settings( const QString& uid )
    {
        settings_map::const_iterator it = registered.find( uid );
        if ( it == registered.end() )
            it = registered.insert( std::make_pair( uid, create_group( uid ) ) ).first;
        
        return it->second;
    }

    ///Create QSettings object from main object and enter to the \p uid group
    settings_ptr create_group( const QString& uid, const QString group = QString() )
    {
        settings_ptr s( new QSettings( settings->fileName(), format_c ) );
        s->beginGroup( ( group.isEmpty() ) ? uid : group );
        return s;
    }

    ///Create QSettings object from \p filename file. If \p relative is \b false then \p filename counts as absolute path.
    settings_ptr create_file( const QString& filename, bool relative )
    {
        QString dir = ( relative ) ? dir_path() + "/" : QString();
        return settings_ptr ( new QSettings( dir + filename, format_c ) );
    }

    settings_ptr settings;
    settings_map registered;
};


settings::settings()
    : p_( new Pimpl )
{}

void settings::register_file(const QString& uid, const QString& filename, bool relative)
{
    if ( filename.isEmpty() )
        p_->registered.erase(uid);
    else 
        p_->registered[uid] = p_->create_file(filename, relative);
}

void settings::register_group(const QString& uid, const QString& group)
{
    if ( group.isEmpty() )
        p_->registered.erase(uid);
    else
        p_->registered[uid] = p_->create_group(uid, group);
}

settings::settings_ptr settings::get_uid(const QString& uid)
{
    return p_->get_settings(uid);
}






