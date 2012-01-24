
#include <stdexcept>

#include <QFile>

#include "../remote.h"
#include "json_file.h"

#include "3rdparty/qjson/serializer.h"
#include "3rdparty/qjson/parser.h"

#include <boost/property_tree/json_parser.hpp>

using namespace std;
using namespace remote;
/*
action* json_file_storage::get(const QString& type)
{
    QFile f(filename_);
    if ( f.open(QFile::ReadOnly ) ) {
        bool ok;
        QJson::Parser parser;
        
        QVariantMap data = parser.parse(&f, &ok).toMap();

        if(!ok)
            throw std::runtime_error("can't parse json object");
        
//         return object(data);
    } else {
        throw std::runtime_error("can't get object");
    }
}

action* json_file_storage::put(const remote::group& obj)
{
    QFile f(filename_);
    if ( f.open(QFile::WriteOnly | QFile::Truncate) ) {
        QJson::Serializer serializer;
        QByteArray s =  serializer.serialize(obj.save());

        //this is holy shit workaround, because of QJson::Serializer doesn't
        //indent output text, but boost::json_parser does!
        std::stringstream ss;
        ss.str(QString(s).toStdString());

        boost::property_tree::ptree ptree;
        boost::property_tree::read_json( ss, ptree );
        boost::property_tree::write_json( ss, ptree );

        f.write( ss.str().data() );
        
    } else {
        throw std::runtime_error("can't put object");
    }
}

*/

QVariantMap remote::from_json(const QByteArray& d)
{
    bool ok = false;
    
    int i = d.indexOf("{");
    
    QByteArray data = d.mid(i, -1);
    
    std::cerr<<"real Code:"<<'{'<<std::endl;
    std::cerr<<"Code:"<<data[0]<<std::endl;
    
    std::cerr<<"DATA:["<<data.constData()<<"]"<<std::endl;
    
    QVariantMap json = QJson::Parser().parse(data, &ok).toMap();
    
    std::cerr<<"size:"<<json.size()<<std::endl;

    if(!ok)
        throw std::runtime_error("can't parse json object");

    return json;    
}


QByteArray remote::to_json(const QVariantMap& map)
{
    QByteArray data = QJson::Serializer().serialize(map);  
    std::stringstream ss;
    ss.str(data.constData());

    boost::property_tree::ptree ptree;
    boost::property_tree::read_json( ss, ptree );
    boost::property_tree::write_json( ss, ptree );

    return QByteArray(ss.str().c_str());
}

