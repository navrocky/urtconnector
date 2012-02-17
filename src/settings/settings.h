#ifndef URT_SETTINGS_H
#define URT_SETTINGS_H

#include <boost/shared_ptr.hpp>
#include <QVariant>

#include <QTemporaryFile>

class QString;
class QSettings;

/*! \brief Main settings holder

Example:

    class qstat_settings: public settings_uid_provider<qstat_settings>
    {
        //members
    }

    main{

        settings main_settings;

        //registering qstat uid to be handled from file.
        main_settings.register_file( qstat_settings::uid(), "qstat.ini" );

        //registering launch uid to be handled from subgroup.
        main_settings.register_file( launch_settings::uid(), "launch" );
    }

    \warning:
    Official Qt: 
    void QSettings::clear() - Removes all entries in the primary location associated to this QSettings object.
    If you only want to remove the entries in the current group(), use remove("") instead.
*/
class base_settings
{
public:

    typedef boost::shared_ptr<QSettings> qsettings_p;

    base_settings(bool use_organization = false);

    ///Register QSettings from \p filename to be accessible by \p uid
    void register_file(const QString& uid, const QString& filename, bool relative = true);

    ///Register QSettings from \p group to be accessible by \p uid
    void register_group(const QString& uid, const QString& group);

    ///Register QSettings from \p group to be accessible by \p uid in \b file 
    void register_group(const QString& uid, const QString& group, const QString& filename, bool relative = true);

    ///Register QSettings from \p group to be accessible by \p uid in settings \b parent_uid
    qsettings_p register_sub_group(const QString& uid, const QString& group, const QString& parent_uid);

    
    void unregister(const QString& uid);

    ///Get main QSettings object
    qsettings_p main();

    ///Get QSettings object registered byt \p uid.
    /*! If no object registered register_group(uid, uid) is called and created object returned */
    static qsettings_p get_settings(const QString& uid);

private:
    struct pimpl;
    boost::shared_ptr<pimpl> p_;
};

struct settings_group {
    explicit settings_group(base_settings::qsettings_p settings, const QString& group);
    ~settings_group();
    
private:
    const base_settings::qsettings_p settings_;
};


///Take value from \p old_settings by \p old_key and put it to \p new_settigns by \p new_key. Old entry completely removed from old_settings
void update_setting_value( base_settings::qsettings_p& old_setings, base_settings::qsettings_p& new_setings, const QString& old_key, const QString& new_key );

base_settings::qsettings_p clone_settings( base_settings::qsettings_p s, const QString& filename );

void copy_settings( base_settings::qsettings_p src, base_settings::qsettings_p dst );

base_settings::qsettings_p fill_settings( base_settings::qsettings_p dst, const QVariantMap& settings );
QVariantMap extract_settings(base_settings::qsettings_p src);

///Base class tu provide uid to any settings objects
template<typename T>
class settings_uid_provider
{
public:
    settings_uid_provider(const base_settings& s = base_settings())
        : s_(s)
        , part_( s_.get_settings(uid()) )
    {}

    base_settings::qsettings_p part() const
    {
        return part_;
    }

    static const QString& uid()
    {
        return uid_;
    }

    void restore()
    {
        copy_settings( s_.get_settings(uid()), part_ );
    }

    void commit()
    {
        if( file_ ) copy_settings( part_, s_.get_settings(uid()) );
    }

    void switch_to_tmp()
    {
        if( !file_ )
        {
            file_.reset( new QTemporaryFile() );
            file_->open();
            part_ = clone_settings( part_, file_->fileName() );
        }
    }

    void switch_from_tmp()
    {
        if( file_ )
        {
            part_ = s_.get_settings(uid());
            file_.reset();
        }
    }

private:
    static QString uid_;
    base_settings s_;

    base_settings::qsettings_p part_;
    boost::shared_ptr<QTemporaryFile> file_;
};

template <typename T>
QString settings_uid_provider<T>::uid_ = typeid (T).name();

//this object created with first instnace of base_settings, and destroyed with QCoreApplication,
// its managed registerd settigns, and flushes them at the end
// this is hack-around about strange bug... 
class qsettings_deleter: public QObject
{
    Q_OBJECT
public:
    qsettings_deleter(QObject* parent) : QObject(parent) {}
    ~qsettings_deleter(){}

public slots:
    void aboutToQuit();
};

#endif
