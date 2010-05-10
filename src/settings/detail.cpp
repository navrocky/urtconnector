
#include <QDir>
#include <QSettings>

#include "detail.h"

//////////////////////////////////////////////////
//           settings_holder implementation
//////////////////////////////////////////////////

QString make_config_path( settings::Scope scope, const QString& org, const QString& app,  const QString& usr_path)
{
    QString tmp_path = settings::path( scope );
    tmp_path += "/"+org+"/"+app + "/" + usr_path;
    QDir dir;
    dir.mkpath(tmp_path);
    return tmp_path;
}

QString make_config_file(const QString& config_path, const QString& file_name )
{
    return config_path + "/" + ( ( file_name.isEmpty() ) ? QDir(config_path).dirName() : file_name ) + ".conf";
}

//Helper struct that creates and dispatching configs
struct settings_holder::HolderPimpl{

    HolderPimpl( const QString& path, const QString& entry, settings::EntryType etype, settings::Scope scope )
        : path(path), entry(entry), etype(etype), scope(scope)
        , config_path( make_config_path( scope, settings::organization(), settings::application(), path ) )
    {
        QString file_name = ( etype == settings::Merge ) ? QString() : entry;
        full_name = make_config_file( config_path, file_name );

        qs.reset( new QSettings( full_name, QSettings::IniFormat ) );

        qs->beginGroup( ( etype == settings::Merge ) ? entry : QString() );
    }

    ~HolderPimpl()
    {
        qs->endGroup();
    }

    QString path;
    QString entry;
    settings::EntryType etype;
    settings::Scope scope;
    QString config_path;
    QString full_name;
    std::auto_ptr<QSettings> qs;
};

settings_holder::settings_holder(const QString& path, const QString& entry, settings::EntryType etype, settings::Scope scope )
    : p_( new HolderPimpl(path, entry, etype, scope) )
{}

settings_holder::~settings_holder()
{}

settings_holder settings_holder::clone() const
{
    return settings_holder( p_->path, p_->entry, p_->etype, p_->scope );
}

QSettings* settings_holder::get() const
{
    return p_->qs.get();
}

const QString& settings_holder::path() const
{
    return p_->path;
}

const QString& settings_holder::entry() const
{
    return p_->entry;
}

settings::EntryType settings_holder::etype() const
{
    return p_->etype;
}

settings::Scope settings_holder::scope() const
{
    return p_->scope;
}

const QString& settings_holder::filename() const
{
    return p_->full_name;
}







