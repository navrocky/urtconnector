#include <QDataStream>
#include <cl/except/error.h>

#include "filter.h"
#include "filter_factory.h"
#include "custom_filter.h"
#include "tools.h"

QByteArray filter_save(filter_p f)
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)2; //version
    if (f)
    {
        ds << f->get_class()->id();
        ds << f->enabled();
        ds << f->name();
        ds << f->save();
    } else
        ds << QString(); // empty class name - no filter

    return res;
}

filter_p filter_load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);
    
    qint32 version;
    ds >> version;
    if (version < 1)
        throw cl::except::error("Invalid filter version");
    
    // create filter
    QString id;
    ds >> id;

    // no filter?
    if (id.isEmpty())
        return filter_p();

    filter_p res = factory->create_filter_by_id(id);
    //if we loading custom filter - we must set factory into it
    custom_filter* custom = qobject_cast<custom_filter*>(res.get());
    if( custom )
        custom->set_factory( factory );

    bool enabled;
    ds >> enabled;
    res->set_enabled(enabled);

    if (version >= 2)
    {
        QString name;
        ds >> name;
        res->set_name(name);
    }
    
    QByteArray ba2;
    ds >> ba2;
    res->load(ba2, factory);
    
    return res;
}

