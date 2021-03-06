
#include <iostream>
#include <stdexcept>

#include <boost/foreach.hpp>

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QSettings>

#include "settings.h"

const QSettings::Format format_c = QSettings::IniFormat;

typedef std::map<QString, base_settings::qsettings_p > settings_map;

class settings_holder: public QObject
{
public:
    static void delete_holder()
    {
        delete instance_;
        instance_ = reinterpret_cast<settings_holder*>(-1);
    }
    
    static settings_map& registered()
    {
        if( instance_ == reinterpret_cast<settings_holder*>(0) )
        {
            instance_ = new settings_holder(qApp);
            connect( qApp, SIGNAL(aboutToQuit()), new qsettings_deleter(qApp), SLOT(aboutToQuit()) );
        }
        else if ( instance_ == reinterpret_cast<settings_holder*>(-1) )
        {
            throw std::logic_error("Using settings after QCoreApplication quits!");
        }
        
        return instance_->registered_;
    }

private:
    settings_holder(QObject* parent): QObject(parent){}
    static settings_holder* instance_;
    settings_map registered_;
};

settings_holder* settings_holder::instance_ = 0;

void qsettings_deleter::aboutToQuit()
{
//    std::cerr << "aboutToQuit" << std::endl;
    settings_holder::delete_holder();
}

struct base_settings::pimpl
{
    pimpl(bool org)
    : settings_(new QSettings(format_c, QSettings::UserScope
                              , (org) ? QCoreApplication::organizationName() : QCoreApplication::applicationName()
                              , QCoreApplication::applicationName()))
    , registered_(settings_holder::registered())
    {}

    ///Get dir to the main QSettings object
    QString dir_path() const
    {
        return QFileInfo(settings_->fileName()).absoluteDir().path() + "/";
    }

    ///Get internal QSettings by \p uid. Group is created if no group found.
    qsettings_p get_settings(const QString & uid)
    {
        settings_map::const_iterator it = registered_.find(uid);
        if (it == registered_.end())
            it = registered_.insert(std::make_pair(uid, create_group(uid))).first;

        return it->second;
    }

    ///Create QSettings object from main object and enter to the \p uid group
    qsettings_p create_group(const QString& uid, const QString group = QString())
    {
        qsettings_p s(new QSettings(settings_->fileName(), format_c));
        s->beginGroup((group.isEmpty()) ? uid : group);
        return s;
    }

    ///Create QSettings object from main object and enter to the \p uid group
    qsettings_p create_group( const QString& uid, const QString group, qsettings_p settings )
    {
        qsettings_p s(new QSettings(settings->fileName(), format_c));
        if( !settings->group().isEmpty() )
            s->beginGroup( settings->group() );
        s->beginGroup((group.isEmpty()) ? uid : group);
        return s;
    }

    ///Create QSettings object from \p filename file. If \p relative is \b false then \p filename counts as absolute path.

    qsettings_p create_file(const QString& filename, bool relative)
    {
        QString dir = (relative) ? dir_path() + "/" : QString();
        return qsettings_p(new QSettings(dir + filename, format_c));
    }

    qsettings_p settings_;
    settings_map& registered_;
};

base_settings::base_settings(bool use_organization)
: p_(new pimpl(use_organization))
{
}

void base_settings::register_file(const QString& uid, const QString& filename, bool relative)
{
    p_->registered_[uid] = p_->create_file(filename, relative);
}

void base_settings::register_group(const QString& uid, const QString& group)
{
    p_->registered_[uid] = p_->create_group(uid, group);
}

void base_settings::register_group(const QString& uid, const QString& group, const QString& filename, bool relative)
{
    p_->registered_[uid] = p_->create_file(filename, relative);
    p_->registered_[uid]->beginGroup(group);
}

void base_settings::register_sub_group(const QString& uid, const QString& group, const QString& parent_uid)
{
    p_->registered_[uid] = p_->create_group(uid, group, get_settings(parent_uid) );
}


void base_settings::unregister(const QString& uid)
{
    p_->registered_.erase(uid);
}

base_settings::qsettings_p base_settings::main()
{
    return p_->settings_;
}

base_settings::qsettings_p base_settings::get_settings(const QString& uid)
{
    base_settings s;
    return s.p_->get_settings(uid);
}

void update_setting_value(base_settings::qsettings_p& old_s, base_settings::qsettings_p& new_s, const QString& old_key, const QString& new_key)
{
    if( !old_s->childKeys().contains( old_key ) )
        return;
    
    new_s->setValue( new_key, old_s->value(old_key) );
    old_s->remove( old_key );
}

base_settings::qsettings_p clone_settings(base_settings::qsettings_p s, const QString& filename)
{
    base_settings::qsettings_p dup( new QSettings( filename, s->format() ) );
    
    BOOST_FOREACH( const QString& key, s->allKeys() )
        dup->setValue( key, s->value(key) );

    dup->sync();
    return dup;
}


void copy_settings(base_settings::qsettings_p src, base_settings::qsettings_p dst)
{
    //previously removing items from destination
    BOOST_FOREACH( const QString& key, dst->allKeys() )
        if( !src->contains(key) ) dst->remove(key);

    //copying valuest from source to destination
    BOOST_FOREACH( const QString& key, src->allKeys() )
        dst->setValue( key, src->value(key) );

    dst->sync();
}

