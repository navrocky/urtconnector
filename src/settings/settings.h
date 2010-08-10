
#ifndef URT_SETTINGS_H
#define URT_SETTINGS_H

#include <boost/shared_ptr.hpp>

///Main settings holder
/*!

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
 
 */
class settings {

public:
    
    typedef boost::shared_ptr<QSettings> settings_ptr;

    settings( bool use_organization = false);

    ///Register QSettings from \p filename to be accessible by \p uid
    void register_file( const QString& uid, const QString& filename, bool relative = true);

    ///Register QSettings from \p group to be accessible by \p uid
    void register_group( const QString& uid, const QString& group);

    ///Get main QSettings object
    settings_ptr main();

    ///Get QSettings object registered byt \p uid.
    /*! If no object registered register_group(uid, uid) is called and created object returned */
    static settings_ptr get_settings( const QString& uid );

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};


///Base class tu provide uid to any settings objects
template<typename T>
class settings_uid_provider{
    static QString uid_;
    settings s;
public:

    settings_uid_provider(const settings& s = settings() )
        : s(s){}

    settings::settings_ptr part()
    { return s.get_settings( uid() ); }

    static const QString& uid()
    { return uid_;}


};

template <typename T>
QString settings_uid_provider<T>::uid_ = typeid(T).name();


#endif
