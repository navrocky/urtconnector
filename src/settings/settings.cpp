//
// C++ Implementation: config
//
// Description:
//
//
// Author: Samoilenko Yuri <kinnalru@gmail.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <map>
#include <stdexcept>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "detail.h"
#include "settings.h"


//////////////////////////////////////////////////
//           static member implementation
//////////////////////////////////////////////////

struct config_internals
{
    std::map<settings::Scope, QString>  path;
    QString organization;
    QString application;
};

static config_internals config_impl;


void settings::set_path(Scope scope, const QString & path)
{
    config_impl.path[scope] = path;
}

void settings::set_organization(const QString & org)
{
    config_impl.organization = org;
}

void settings::set_application(const QString & app)
{
    config_impl.application = app;
}

QSettings::Scope q_scope(settings::Scope scope)
{
    switch(scope)
    {
        case settings::User: return QSettings::UserScope; break;
        case settings::System: return QSettings::SystemScope; break;
        default: throw std::runtime_error("Something wrong with QSettings::Scope");
    }
}

QString settings::path(Scope scope)
{
    //Out internal path
    QString path( config_impl.path[scope] );
    //Path to the file QSettings refferenced by
#ifdef Q_WS_WIN
    QString qfilepath = QSettings( QSettings::IniFormat, q_scope(scope), organization() ).fileName();
#else
    QString qfilepath = QSettings( q_scope(scope), organization() ).fileName();
#endif
    //Path ti the folder
    QString qpath = QFileInfo(qfilepath).absoluteDir().path();

    return ( path.isEmpty() ) ? qpath : path;
}

QString settings::organization()
{
    QString org( config_impl.organization );
    return ( org.isEmpty() ) ? QCoreApplication::organizationName() : org;
}

QString settings::application()
{
    QString app( config_impl.application );
    return ( app.isEmpty() ) ? QCoreApplication::applicationName() : app;
}




//////////////////////////////////////////////////
//           settings implementation
//////////////////////////////////////////////////


struct settings::Pimpl {

    Pimpl(const QString & path, const QString & entry, EntryType etype, Scope scope)
        :holder(path, entry, etype, scope)
    {}

    Pimpl(const settings_holder& holrer)
        :holder(holder)
    {}
    
    settings_holder holder;
};


settings::settings(Scope scope)
    : p_(  new Pimpl( QString(), QString(), Merge, scope ) )
{}

settings::settings(const QString & entry, EntryType etype, Scope scope)
    : p_(  new Pimpl( QString(), entry, etype, scope ) )
{}

settings::settings(const QString & path, const QString & entry, EntryType etype, Scope scope)
    : p_(  new Pimpl( path, entry, etype, scope ) )
{}

settings::settings(const settings_holder& holder)
    : p_(  new Pimpl( holder ) )
{}


settings::~settings()
{}

settings settings::clone() const
{
    return settings( p_->holder.clone() );
}


const QString & settings::path() const
{
    return p_->holder.path();
}

const QString & settings::filename() const
{
    return p_->holder.filename();
}

settings::Scope settings::scope() const
{
    return p_->holder.scope();
}

settings settings::entry( const QString & entry, EntryType etype ) const
{
    return settings( p_->holder.path(), entry, etype, p_->holder.scope() );
}

QVariant settings::value(const QString & key, const QVariant & defaultValue) const
{
    QVariant ret = p_->holder.get()->value(key, defaultValue);
    if ( ret.isValid() )
        return ret;

    if ( scope() == User )
    {
        //Using fallback and trying to return system value
        Pimpl system_config( p_->holder.path(), p_->holder.entry(), p_->holder.etype(), System );
        return system_config.holder.get()->value( key, defaultValue );
    }
    else
    {
        return defaultValue;
    }
}

void settings::set_value(const QString & key, const QVariant & defaultValue)
{
    if ( !p_->holder.get()->isWritable() )
        throw std::runtime_error("Can't write to non-writable config file!");

    p_->holder.get()->setValue(key, defaultValue);
}

void settings::sync()
{
    if ( p_ ) p_->holder.get()->sync();
}

void settings::remove(const QString& key)
{
    p_->holder.get()->remove(key);
}



