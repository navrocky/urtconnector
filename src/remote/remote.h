
#ifndef URT_REMOTE_H
#define URT_REMOTE_H

#include <boost/foreach.hpp>
#include <boost/tr1/tuple.hpp>

#include <set>
#include <stdexcept>

#include <qt4/QtCore/QDateTime>
#include <qt4/QtCore/QVariant>

#include "remote/backends/binary_file.h"
#include "remote/backends/json_file.h"

namespace remote {

    
/*! if you want object to allow sync, subclass from this */
class syncable {
public:

    const static QString deleted_key;
    const static QString stamp_key;
    const static QString id_key;

    syncable( bool deleted = false, const QDateTime& st = QDateTime::currentDateTime() )
        : deleted_(deleted), stamp_(st)
    {}

    /*! is object marked to be deleted? */
    inline bool is_deleted() const { return deleted_; };
    /*! mark object to delete */
    inline void set_deleted() { deleted_ = true; };

    /*! get timestamp of object */
    inline const QDateTime& sync_stamp() const { return stamp_; };
    /*! set timestamp of object */
    inline void set_sync_stamp(const QDateTime& stamp = QDateTime::currentDateTime()) { stamp_ = stamp; };

    /*! id must uniquely identify object within object collection*/
    virtual QString sync_id() const = 0;

    /*! serialize object*/
    virtual QVariantMap save() const = 0;
    /*! deserialize object*/
    virtual void load(const QVariantMap& data) = 0;

private:
    bool deleted_;
    QDateTime stamp_;
};


/// help function to check VariatnMap for sanity
inline const QVariantMap& check_data( const QVariantMap& data) {
    if (   !data.contains(syncable::deleted_key)
        || !data.contains(syncable::stamp_key)
        || !data.contains(syncable::id_key) )
        throw std::runtime_error("illegal intermediate data!");
    return data;
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

    virtual QString sync_id() const { return id_; };
    virtual QVariantMap save() const { return data_; };

    bool operator< (const intermediate& other) const { return id_ < other.sync_id(); }

private:
    virtual void load(const QVariantMap& data){};

private:
    QString id_;
    QVariantMap data_;
};

// you can provide your own convert functions

QVariantMap to_variantmap(const syncable& s);
QVariantMap to_variantmap(const QVariant& v);

/*! this class represents logic object to communicate with remote storage */
class group {
public:
    typedef std::set<intermediate> Entries;

    group(const QString& type, const Entries& entries = Entries());
    group(const QVariantMap& data);

    inline const QString& type() const { return type_; }
    inline const Entries& entries() const { return entries_; }

    template<typename T>
    group& operator<< ( const T& t) {
        entries_.insert( to_variantmap(t) );
        return *this;
    }

    virtual QVariantMap save() const;

    virtual void load(const QVariantMap& data);

private:
    QString type_;
    Entries entries_;
};


/*! merge two intermediate sets*/
inline group::Entries merge(const group::Entries& e1, const group::Entries& e2 ) {
    group::Entries ret = e1;

    bool inserted;
    group::Entries::iterator it;
    BOOST_FOREACH(const intermediate& imd, e2) {
        std::tr1::tie(it, inserted) = ret.insert(imd);
        if( !inserted && it->sync_stamp() < imd.sync_stamp() ) {
            const intermediate& tmp = *it;
            const_cast<intermediate&>(tmp) = imd; //it is guaranteed that order of items remains unchanged
        }
    }

    return ret;
}
    
} //namespace remote

#endif

