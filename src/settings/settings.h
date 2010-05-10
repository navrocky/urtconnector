//
// C++ Interface: config
//
// Description:
//
//
// Author: Samoilenko Yuri <kinnalru@gmail.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef URT_SETTINGS_H
#define URT_SETTINGS_H

#include <map>

#include <boost/shared_ptr.hpp>

#include <QVariant>

class settings_holder;

///Wrapper above QSettings with extended functianality
class settings {

public:
   
    typedef QString  key_type;
    typedef QVariant data_type;

    ///Scope where config file located
    enum Scope {
        User,
        System
    };

    ///Type of entry creation
    enum EntryType{
        ///Create entry IN current config
        Merge,
        ///Create entry in separate config file
        Split
    };

    ///Set path to config file in selected space
    static void set_path( Scope scope, const QString& path );
    
    ///Set orgtanization name( QCoreApplication::organizationName() is default )
    static void set_organization( const QString& org );
    
    ///Set application name( QCoreApplication::applicationName() is default )
    static void set_application( const QString& app );

    ///Path where config stored( QSettings(scoe).path() is default )
    static QString path( Scope scope );
    
    ///Orgtanization name( QCoreApplication::organizationName() is default )
    static QString organization();
    
    ///Orgtanization name( QCoreApplication::applicationName() is default )
    static QString application();

    /*!\brief Create settings
        Config is placed to path "Scope/organization/application/application.conf"
    */
    settings(Scope scope = User);


    /*!\brief Create sub-config
        - If entry type is \b Merge, than config will be a section in QSettings( beginGroup("entry") ) \n
            "Scope/organization/application/application.conf"
        - If type is \b Split, than config will be a separate file store by path \n
            "Scope/organization/application/application/entry.conf"
    */
    settings( const QString& entry, EntryType etype = Merge, Scope scope = User);

    ~settings();

    settings clone() const;
    
    ///Get path where config located
    const QString& path() const;

    ///Get name of the config file
    const QString& filename() const;

    ///Get the scope where config located
    Scope scope() const;

    ///Create config as sub-config of the current
    settings entry( const QString& entry, EntryType etype = Merge ) const;

    ///Get value from current config by key \b key
    QVariant value( const QString & key, const QVariant & defaultValue = QVariant() ) const;

    template <typename T>
    T value( const QString & key ) const
    {
        return value(key).value<T>();
    }

    template <typename T>
    T value( const QString & key, const T& defaultValue ) const
    {
        return value(key, qVariantFromValue(defaultValue) ).value<T>();
    }
    
    ///Set value in current config
    void set_value( const QString & key, const QVariant & value );

    /*!Writes any unsaved changes to permanent storage, and reloads any settings that have been changed in the meantime by another application
        This function is called automatically from settings's destructor
    */
    void sync();

    void remove(const QString& key); 

private:

    settings( const settings_holder& holder);
    
    /*!\brief Create sub-config in selected path
        - If entry type is \b Merge, than config will be a section in QSettings( beginGroup("entry") ) \n
            "Scope/organization/application/path/dirname.conf"
        - If type is \b Split, than config will be a separate file store by path \n
            "Scope/organization/application/path/entry.conf"
    */
    settings( const QString& path, const QString& entry, EntryType etype = Merge , Scope scope = User);
    
private:
    friend class settings_list;
    friend class settings_array_iterator;
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;

};

#include "detail.h"


#endif


