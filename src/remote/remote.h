
#ifndef URT_REMOTE_H
#define URT_REMOTE_H

#include <boost/foreach.hpp>
#include <boost/tr1/tuple.hpp>

#include <set>
#include <stdexcept>

// #include <QDateTime>
#include <qt4/QtCore/QDateTime>
#include <qt4/QtCore/QFile>
#include <qt4/QtCore/qtextstream.h>
#include <qt4/QtCore/QVariant>

#include <iostream>
#include <boost/property_tree/json_parser.hpp>

namespace remote {
    
    struct serializable {
        /*! serialize object*/
        virtual QVariantMap save() const = 0;
        /*! deserialize object*/
        virtual void load(const QVariantMap& data) = 0;        
    };
    
    /*! if you want object to allow sync, subclass from this */
    class syncable: public serializable {
    public:

        const static QString deleted_key;
        const static QString stamp_key;
        const static QString id_key;
        
        syncable( bool deleted = false, const QDateTime& st = QDateTime::currentDateTime() )
            : deleted_(deleted), stamp_(st)
        {}
        
        /*! is object marked to be deleted? */
        bool is_deleted() const { return deleted_; };
        /*! mark object to delete */
        void set_deleted() { deleted_ = true; };
        
        /*! get timestamp of object */
        const QDateTime& sync_stamp() const { return stamp_; };
        /*! update object's timestamp */
        void sync_touch() { stamp_ = QDateTime::currentDateTime(); }
        
        /*! id must uniquely identify object within object collection*/
        virtual QString sync_id() const = 0;
        
    private:
        bool deleted_;
        QDateTime stamp_;
    };

   
    /// help function to check VariatnMap for sanity
    inline const QVariantMap& check_data( const QVariantMap& data) {
        if (   !data.contains(syncable::deleted_key)
            || !data.contains(syncable::stamp_key)
            || !data.contains(syncable::id_key) )
            throw std::runtime_error("illegal data!");
        return data;
    }
    
    /// help function to create sanity VariantMap
    inline QVariantMap create_data( const QVariantMap& data, bool deleted, const QDateTime& dt, const QString& id) {
        QVariantMap ret = data;
        ret[syncable::deleted_key] = deleted;
        ret[syncable::stamp_key]   = dt;
        ret[syncable::id_key]      = id;
        return ret;
    }

    /*! intermediate object representation */
    class intermediate: public syncable {
    public:
        
        /// Construct from serialized data
        intermediate(const QVariantMap& data)
            : syncable( data[deleted_key].toBool(), data[stamp_key].toDateTime() )
            , id_( data[id_key].toString() )
            , data_( check_data(data) )
        {}

        
        /// Construct from origal data
        intermediate(const syncable& s)
            : syncable( s.is_deleted(), s.sync_stamp() )
            , id_( s.sync_id() )
            , data_( create_data(s.save(), is_deleted(), sync_stamp(), id_) )
        {}

        virtual QString sync_id() const { return id_; };
        virtual QVariantMap save() const { return data_; };
        
        bool operator< (const intermediate& other) const { return id_ < other.sync_id(); }
        
    private:
        virtual void load(const QVariantMap& data){};
        
    private:
        QString id_;
        QVariantMap data_;
    };
    
    
    /*! this class represents logic object to communicate with remote storage */
    class object {
    public: 
        typedef std::set<intermediate> Entries;
        
        
        object(const QString& t): type_(t) {}
        object(const QVariantMap& data) { load(data); }
        
        const QString& type() const { return type_; }
        const Entries& entries() const { return entries_; }
        
        object& operator<< ( const intermediate& imd) {
            entries_.insert( imd );
            return *this;
        }
        
        QVariantMap save() const {
            QVariantMap ret;
            ret["type"] = type_;
            QVariantList ls;
            BOOST_FOREACH( const intermediate& imd, entries_ ) {
                ls << imd.save();
            }
            ret["data"] = ls;
            return ret;
        }
        
        void load( const QVariantMap& data) {
            type_ = data["type"].toString();
            
            BOOST_FOREACH( const QVariant& v, data["data"].toList() ) {
                intermediate imd(v.toMap());
                entries_.insert(imd);
            }
        }
        
    private:
        QString type_;
        Entries entries_;                
    };
    
    

    /*! backend interface */
    struct storage {
        virtual object get( const QString& type ) = 0;
        virtual void put( const object& obj ) = 0;
    };
    
    
    /*! simple test backend*/
    class binary_file_storage: public storage {
    public:
        
        binary_file_storage(const QString& filename): filename_(filename) {}
        
        virtual object get( const QString& type ){
            QFile f(filename_);
            if ( f.open(QFile::ReadOnly ) ) {
                QDataStream stream(&f);
                QVariantMap data;
                
                stream >> data;

                return object(data);
            } else {
                throw std::runtime_error("can't get object");
            }
        }
        
        virtual void put( const object& obj ){
            QFile f(filename_);
            if ( f.open(QFile::WriteOnly | QFile::Truncate) ) {
                QDataStream stream(&f);
                stream << obj.save();
            } else {
                throw std::runtime_error("can't put object");
            }
        }
        
    private:
        QString filename_;
    };
    
    /*! merge two intermediate sets*/
    inline object::Entries merge(const object::Entries& e1, const object::Entries& e2 ) {
        object::Entries ret = e1;
        
        bool inserted;
        object::Entries::iterator it;
        BOOST_FOREACH(const intermediate& imd, e2) {
            std::tr1::tie(it, inserted) = ret.insert(imd);
            if( !inserted && it->sync_stamp() < imd.sync_stamp() ) {
                const intermediate& tmp = *it;
                const_cast<intermediate&>(tmp) = imd; //it is guaranteed that order of items remains unchanged
            }
        }
        
        return ret;
    }
    
    
    
    
    
}



#endif

