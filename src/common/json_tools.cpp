
#include <boost/property_tree/json_parser.hpp>

#include "3rdparty/qjson/serializer.h"
#include "3rdparty/qjson/parser.h"

#include "json_tools.h"

QVariantMap from_json(const QByteArray& d)
{
    bool ok = false;

    //truncate trash before json
    QByteArray data = d.mid(d.indexOf("{"), -1);

    QVariantMap json = QJson::Parser().parse(data, &ok).toMap();

    if(!ok)
        throw std::runtime_error("can't parse json object");

    return json;
}


QByteArray to_json(const QVariantMap& map)
{
    QByteArray data = QJson::Serializer().serialize(map);
    std::stringstream ss;
    ss.str(data.constData());

    boost::property_tree::ptree ptree;
    boost::property_tree::read_json( ss, ptree );
    boost::property_tree::write_json( ss, ptree );

    return QByteArray(ss.str().c_str());
}
