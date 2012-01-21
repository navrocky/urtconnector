
#include <algorithm>

#include <boost/bind.hpp>

#include "remote.h"

using namespace remote;

const QString remote::syncable::deleted_key = "sync_deleted";
const QString remote::syncable::stamp_key   = "sync_stamp";
const QString remote::syncable::id_key      = "sync_id";

QVariantMap remote::to_variantmap(const syncable& s)
{
    QVariantMap data( s.save() );
    data[syncable::deleted_key] = s.is_deleted();
    data[syncable::stamp_key]   = s.sync_stamp();
    data[syncable::id_key]      = s.sync_id();

    return data;
}

QVariantMap remote::to_variantmap(const QVariant& v)
{ return v.toMap(); }



group::group(const QString& type, const Entries& entries /*= Entries()*/)
    : type_(type)
    , entries_(entries)
{}

group::group(const QVariantMap& data)
{ load(data); }


QVariantMap remote::group::save() const
{
    QVariantMap ret;

    QVariantList list;
    std::transform(entries_.begin(), entries_.end(),
        std::back_inserter(list), boost::bind(&syncable::save, _1));

    ret.insert("type", type_);
    ret.insert("data", list);

    return ret;
}

void group::load(const QVariantMap& data)
{
    type_ = data["type"].toString();

    QVariant d = data["data"];
    QVariantList list = d.toList();
    
    QVariantMap(*ptr)(const QVariant&) = to_variantmap;
    std::transform(list.begin(), list.end(),
        std::inserter(entries_, entries_.end()), boost::bind(ptr, _1));
    
    std::cerr<<"esize:"<<entries_.size()<<std::endl;
    std::cerr<<"E:"<<entries_.begin()->sync_stamp().toString().toStdString()<<std::endl;    
    
}

