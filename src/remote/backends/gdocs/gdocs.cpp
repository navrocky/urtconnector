
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

typedef QPair<QString, QString> QueryArg;
typedef QList<QueryArg>         QueryList;

static const char* property_c = "ctx_property";

static const QUrl login_uri_c = QString("https://www.google.com/accounts/ClientLogin");

//API2
//const QUrl search_uri_c = QString("http://docs.google.com/feeds/documents/private/full");

//API3
static const QUrl base_uri_c       = QString("https://docs.google.com/feeds/default/private/full");
static const QUrl download_uri_c   = QString("https://docs.google.com/feeds/download/documents/Export");



struct request_context {
    QString         name;
    gdocs::Action   action;
    QString         url;
};

Q_DECLARE_METATYPE(request_context);

gdocs::gdocs(const QString& login, const QString& password, const QString& app_name, QObject* parent)
    : QObject(parent)
    , state_(None)
    , auth_(false)
    , manager_(new QNetworkAccessManager(this))
{
    generic_header_["User-Agent"]    = app_name.toUtf8();
    generic_header_["GData-Version"] = "3.0";
 
    connect(manager_, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(ssl_errors(QNetworkReply*,QList<QSslError>)));
    connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    
//     QStringList auth_data;
//     auth_data  << QString("Email=%1").arg(login)
//                << QString("Passwd=%1").arg(password)
//                << QString("accountType=%1").arg("HOSTED_OR_GOOGLE")
//                << QString("source=%1").arg(app_name)
//                << QString("service=%1").arg("writely");

    QUrl url(login_uri_c);
    url.addQueryItem("Email",       login);
    url.addQueryItem("Passwd",      password);
    url.addQueryItem("accountType", "HOSTED_OR_GOOGLE");
    url.addQueryItem("source",      app_name);
    url.addQueryItem("service",     "writely");
    
    QNetworkRequest auth_request(url);
    fill_header(auth_request);
    
    QNetworkReply* auth_reply = manager_->get(auth_request);

    request_context rc;
    rc.name = "Authentification";
    rc.action = Auth;
    
    auth_reply->setProperty(property_c, qVariantFromValue(rc));
  
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
    request_context request = reply->property(property_c).value<request_context>();

    QByteArray data = reply->readAll();

    std::cerr<<"Reply received:"<<request.name.toStdString()<<std::endl;
    
    std::cerr<<"Reply attr1:"<<reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt()<<std::endl;
    std::cerr<<"Reply attr2:"<<reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toStdString()<<std::endl;
    QString redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().toString();
    std::cerr<<"Reply attr3:"<<redirect.toStdString()<<std::endl;
    std::cerr<<"Reply attr4:"<<reply->attribute(QNetworkRequest::CustomVerbAttribute).toString().toStdString()<<std::endl;
    std::cerr<<"Reply attr5:"<<reply->attribute(QNetworkRequest::AuthenticationReuseAttribute).toInt()<<std::endl;
    
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
                    std::cerr<<"Finded id:"<<d.id.toStdString()<<std::endl;
                    std::cerr<<"Finded src:"<<d.src.toStdString()<<std::endl;
                    break;
                }
//                 break;
                request_context r;
                r.name = QString("Downloading %1").arg(ddd.filename);
                r.action = Download;

                QUrl url(download_uri_c);
                url.setQueryItems( QueryList()
                    //by API 3.0 documentation we must do "docID", but it leads to redirect response. If it dosn't work use "id" key.
                    //<< qMakePair("id", ddd.id)
                    << QueryArg("docID",       ddd.id)
                    << QueryArg("exportFormat","txt" )
                    << QueryArg("formatormat", "txt" )
                );
                
                get(url, r);
                
            }
                        
            
            break;
            
        case Download:;

            std::cerr<<"HERE"<<std::endl;
            if (!redirect.isEmpty()) {
                QUrl url(redirect);

                QNetworkRequest down_request(url);
                fill_header(down_request);

                std::cerr<<"Redirect:="<<down_request.url().toString().toStdString()<<std::endl;
                
                QNetworkReply* down_reply = manager_->get(down_request);

                down_reply->setProperty(property_c, qVariantFromValue(request));
            }
            
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

        QString u = base_uri_c.toString();

        u += "?title="+downloads[i];

        QUrl url(u);
        
        QNetworkRequest docs_request(url);
        fill_header(docs_request);
//         docs_request.setRawHeader("q", downloads[i].toUtf8());
//         docs_request.setRawHeader("title-exact", "true");

        QNetworkReply* docs_reply = manager_->get(docs_request);

        std::cerr<<"URL:="<<docs_request.url().toString().toStdString()<<std::endl;
        
        docs_reply->setProperty(property_c, qVariantFromValue(r));
    }

    downloads.clear();
}

void gdocs::load(const QString& filename)
{
    downloads.push_back(filename);;
}


QNetworkReply* gdocs::get(const QUrl& url, const request_context& c)
{
    QNetworkRequest request(url);
    fill_header(request);

    request_context ctx(c);
    ctx.url = request.url().toString();

    QNetworkReply* reply = manager_->get(request);
    reply->setProperty(property_c, qVariantFromValue(ctx));
    return reply;
}





