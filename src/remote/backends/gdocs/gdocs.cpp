
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>

#include <QString>
#include <QStringList>

#include <cl/syslog/syslog.h>

#include "common/tools.h"

#include <iostream>

#include "gdocs.h"
#include <boost/concept_check.hpp>

SYSLOG_MODULE(gdocs);

typedef qt_signal_wrapper qsw;

const QUrl login_uri_c = QString("https://www.google.com/accounts/ClientLogin");
const QUrl search_uri_c = QString("http://docs.google.com/feeds/documents/private/full");
const QString download_uri_c = "http://docs.google.com/feeds/download/documents/Export";


struct request_context {
    QString name;
    gdocs::Action action;
};

Q_DECLARE_METATYPE(request_context);

gdocs::gdocs(const QString& login, const QString& password, const QString& app_name, QObject* parent)
    : QObject(parent)
    , state_(None)
    , auth_(false)
    , manager_(new QNetworkAccessManager(this))
{
    generic_header_["User-Agent"]    = app_name.toUtf8();
    generic_header_["GData-Version"] = "2.0";
 
    connect(manager_, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(ssl_errors(QNetworkReply*,QList<QSslError>)));
    connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    
    QStringList auth_data;
    auth_data  << QString("Email=%1").arg(login)
               << QString("Passwd=%1").arg(password)
               << QString("accountType=%1").arg("HOSTED_OR_GOOGLE")
               << QString("source=%1").arg(app_name)
               << QString("service=%1").arg("writely");

    QNetworkRequest auth_request(login_uri_c);
    fill_header(auth_request);
    
    QNetworkReply* auth_reply = manager_->post(auth_request, auth_data.join("&").toUtf8());

    request_context rc;
    rc.name = "Authentification";
    rc.action = Auth;
    
    auth_reply->setProperty("test", qVariantFromValue(rc));
  
}

gdocs::~gdocs()
{}

bool gdocs::parse_auth( const QString& data)
{
    QString auth= data.section("Auth=", 1).trimmed();
    generic_header_["Authorization"] = QString("GoogleLogin auth=%1").arg(auth).toUtf8();

    return !auth.isEmpty();
}

void gdocs::fill_header(QNetworkRequest& request)
{
    BOOST_FOREACH(const Headers::value_type& h, generic_header_) {
        request.setRawHeader(h.first, h.second);
    }
}

void gdocs::ssl_errors(QNetworkReply* reply, const QList<QSslError>& errors)
{
    LOG_DEBUG << "ssl_errors encountered:";
    BOOST_FOREACH(const QSslError& err, errors) {
        LOG_DEBUG << err.errorString().toStdString();
    }
    
}

void gdocs::finished(QNetworkReply* reply)
{
    request_context request = reply->property("test").value<request_context>();

    QByteArray data = reply->readAll();

    std::cerr<<"Reply received:"<<request.name.toStdString()<<std::endl;
    std::cerr<<"Reply data:"<<data.constData()<<std::endl;
    
    if (reply->error() != QNetworkReply::NoError) {
        std::cerr<<"Error: "<<request.name.toStdString() << " Action:" << request.action<<std::endl;
    }

    switch (request.action) {
        case Auth:
            if ( parse_auth(data) ) {
                state_ |= Auth;
            }
            break;
        case Query:
            {
                document ddd;
                foreach(const document& d, gdocs_documents(data).documents()) {
                    ddd = d;
                    std::cerr<<"Finded file:"<<d.filename.toStdString()<<std::endl;
                    break;
                }

                request_context r;
                r.name = QString("Downloading %1").arg(ddd.filename);
                r.action = Download;

                QString u = "http://docs.google.com/feeds/download/documents/Export?docID=" + ddd.id + "&exportFormat=txt";
//                 QString u = "http://docs.google.com/feeds/download/documents/exports/Export?exportFormat=txt&amp;id=" + ddd.id;


// "http://docs.google.com/feeds/download/documents/Export?docID=1h4len8THmVOfAhFtsVRoxBC9TifdxYsYjFgZ1NSudr4&exportFormat=txt"
// "http://docs.google.com/feeds/download/documents/Export?exportFormat=txt&amp;id=1h4len8THmVOfAhFtsVRoxBC9TifdxYsYjFgZ1NSudr4">here</A>."

                
                QUrl url(u);

                QNetworkRequest down_request(url);
                fill_header(down_request);
        //         docs_request.setRawHeader("q", downloads[i].toUtf8());
        //         docs_request.setRawHeader("title-exact", "true");

                    std::cerr<<"URL:="<<down_request.url().toString().toStdString()<<std::endl;
                
                QNetworkReply* down_reply = manager_->get(down_request);

                down_reply->setProperty("test", qVariantFromValue(r));
                
            }
                        
            
            break;
            
        case Download:;

            
        default:
            ;
    }

    if ( !(state_ & Auth) )
    {
        return;
    }

    for ( int i = 0; i < downloads.size(); ++i) {
        request_context r;
        r.name = QString("Quering %1").arg(downloads[i]);
        r.action = Query;

        QString u = search_uri_c.toString();

        u += "?title="+downloads[i];

        QUrl url(u);
        
        QNetworkRequest docs_request(url);
        fill_header(docs_request);
//         docs_request.setRawHeader("q", downloads[i].toUtf8());
//         docs_request.setRawHeader("title-exact", "true");

        QNetworkReply* docs_reply = manager_->get(docs_request);

        std::cerr<<"URL:="<<docs_request.url().toString().toStdString()<<std::endl;
        
        docs_reply->setProperty("test", qVariantFromValue(r));
    }

    downloads.clear();
}

void gdocs::load(const QString& filename)
{
    downloads.push_back(filename);;
}





