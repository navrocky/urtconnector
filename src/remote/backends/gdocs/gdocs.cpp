
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QPointer>

#include <QString>
#include <QStringList>

#include <iostream>

#include <cl/syslog/syslog.h>

#include "common/tools.h"

#include "gdocs.h"
#include "../json_file.h"
#include <remote.h>



SYSLOG_MODULE(gdocs);

typedef std::map<QByteArray, QByteArray> Headers;

typedef QPair<QString, QString> QueryArg;
typedef QList<QueryArg>         QueryList;

typedef qt_signal_wrapper qsw;

static const char* action_property_c = "action_property";

static const QUrl login_uri_c = QString("https://www.google.com/accounts/ClientLogin");

//API2
//const QUrl search_uri_c = QString("http://docs.google.com/feeds/documents/private/full");

//API3
static const QUrl base_uri_c       = QString("https://docs.google.com/feeds/default/private/full");
static const QUrl download_uri_c   = QString("https://docs.google.com/feeds/download/documents/Export");

#define GDOCS_DEBUG LOG_DEBUG << " >> " << __PRETTY_FUNCTION__ << ": "

class gdocs_action: public remote::action {
public:
    gdocs_action(std::auto_ptr<context> ctx, gdocs* manager) : ctx(ctx), manager(manager){};
    virtual ~gdocs_action(){};

    virtual void start() {
        Q_ASSERT(!manager.isNull());
        
        manager->start(ctx, std::auto_ptr<gdocs_action>(this)); //dropping ownership
    };
    
    virtual void abort() {
        
    };
    
    friend class gdocs;
    std::auto_ptr<context> ctx;
    QPointer<gdocs> manager;
};




typedef boost::function<void (ContextPtr, const QByteArray&)> Processor;

struct context {
    context(int id, const remote::group& ojb): id(id), obj(ojb) {};
    
    const int id;
    remote::group obj;
    QString filename;
    
    QString auth;
    document doc;
    Headers http_headers;
    QList<Processor> processors;
    std::auto_ptr<gdocs_action> pending;
};

Q_DECLARE_METATYPE(ContextPtr);

namespace {
template <typename T>
void introspect(const T* r) {
    LOG_HARD << "introspecting reply/request:";
    LOG_HARD << "Reply HttpStatusCodeAttribute: "      << r->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    LOG_HARD << "Reply HttpReasonPhraseAttribute: "    << r->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString().toStdString();
    LOG_HARD << "Reply RedirectionTargetAttribute: "   << r->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl().toString().toStdString();
    LOG_HARD << "Reply ConnectionEncryptedAttribute: " << r->attribute(QNetworkRequest::ConnectionEncryptedAttribute).toBool();
    LOG_HARD << "Request CacheLoadControlAttribute: "  << r->attribute(QNetworkRequest::CacheLoadControlAttribute).toInt();
    LOG_HARD << "Request CacheSaveControlAttribute: "  << r->attribute(QNetworkRequest::CacheSaveControlAttribute).toBool();
    LOG_HARD << "Reply SourceIsFromCacheAttribute: "   << r->attribute(QNetworkRequest::SourceIsFromCacheAttribute).toBool();
    LOG_HARD << "Request DoNotBufferUploadDataAttribute: " << r->attribute(QNetworkRequest::DoNotBufferUploadDataAttribute).toBool();
    LOG_HARD << "Request HttpPipeliningAllowedAttribute: " << r->attribute(QNetworkRequest::HttpPipeliningAllowedAttribute).toBool();
    LOG_HARD << "Reply HttpPipeliningWasUsedAttribute: " << r->attribute(QNetworkRequest::HttpPipeliningWasUsedAttribute).toBool();
    LOG_HARD << "Request CustomVerbAttribute:"         << r->attribute(QNetworkRequest::CustomVerbAttribute).toString().toStdString();
    LOG_HARD << "Request CookieLoadControlAttribute: " << r->attribute(QNetworkRequest::CookieLoadControlAttribute).toInt();
    LOG_HARD << "Request CookieSaveControlAttribute: " << r->attribute(QNetworkRequest::CookieSaveControlAttribute).toInt();
    LOG_HARD << "Request AuthenticationReuseAttribute: " << r->attribute(QNetworkRequest::AuthenticationReuseAttribute).toInt();
    
    BOOST_FOREACH(const QNetworkReply::RawHeaderPair& header, r->rawHeaderPairs()){
        LOG_HARD << header.first.constData() << ": " << header.second.constData();
    }
    
}

void introspect(const document& doc) {
    LOG_HARD << "introspecting document:";
    LOG_HARD << "id: "      << doc.id.toStdString();
    LOG_HARD << "src: "     << doc.src.toStdString();
    LOG_HARD << "filename: "<< doc.filename.toStdString();
}

void introspect(const ContextPtr& act) {
    LOG_HARD << "introspecting action:";
    LOG_HARD << "id: "      << act->id;
    LOG_HARD << "filename: "<< act->filename.toStdString();
    LOG_HARD << "auth: "    << act->auth.toStdString();
    LOG_HARD << "procs: "   << act->processors.size();
    introspect(act->doc);
}
}

gdocs::gdocs(const QString& login, const QString& password, const QString& app_name, QObject* parent)
    : QObject(parent)
    , manager_(new QNetworkAccessManager(this))
    , login_(login)
    , password_(password)
    , app_name_(app_name)
    , id_(0)
{
    connect(manager_, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
            SLOT(authentication_required(QNetworkReply*, QAuthenticator*)));

    connect(manager_, SIGNAL(finished(QNetworkReply*)),
            SLOT(finished(QNetworkReply*)));

    //I dislike to pass QNetworkAccessManager::NetworkAccessibility declaration to outer header
    //this hack translates QNetworkAccessManager::NetworkAccessibility into simple int
    connect(manager_, SIGNAL(networkAccessibleChanged(QNetworkAccessManager::NetworkAccessibility)),
            new qsw(manager_, boost::bind(&gdocs::network_accessible_changed, this, boost::bind(&QNetworkAccessManager::networkAccessible, manager_))),
            SLOT(activate()));

    connect(manager_, SIGNAL(proxyAuthenticationRequired(const QNetworkProxy&, QAuthenticator*)),
            SLOT(proxy_authentication_required(const QNetworkProxy&, QAuthenticator*)));
    
    connect(manager_, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(ssl_errors(QNetworkReply*,QList<QSslError>)));
}

gdocs::~gdocs()
{}

remote::action* gdocs::get(const QString& type)
{
    GDOCS_DEBUG << type.toStdString();

    std::auto_ptr<gdocs_action> act = create_action(remote::group(type));

    act->ctx->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2)
        << boost::bind(&gdocs::download_impl, this, _1, _2)
        << boost::bind(&gdocs::process_download, this, _1, _2);

    LOG_DEBUG << "Processors size: " << act->ctx->processors.size();
        
    return act.release();
}

remote::action* gdocs::put(const remote::group& obj)
{
    GDOCS_DEBUG << obj.type().toStdString();
    
    std::auto_ptr<gdocs_action> act = create_action(obj);
    
    act->ctx->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2)
        << boost::bind(&gdocs::upload_impl, this, _1, _2);

    return act.release();
}

remote::action* gdocs::check(const QString& type)
{
    GDOCS_DEBUG << type.toStdString();
    
    std::auto_ptr<gdocs_action> act = create_action(remote::group(type));
    
    act->ctx->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2);

    return act.release();
}

void gdocs::authentication_required(QNetworkReply* reply, QAuthenticator* authenticator) const
{
    LOG_DEBUG << "authentication_required encountered:";
    ContextPtr act = reply->property(action_property_c).value<ContextPtr>();
    introspect(reply);
    introspect(act);
}

void gdocs::finished(QNetworkReply* reply)
{
    ContextPtr ctx = reply->property(action_property_c).value<ContextPtr>();
    LOG_DEBUG << "Reply id: %1, is finished: ", ctx->id, reply->isFinished();
    introspect(ctx);
    introspect(reply);
    
    if ((reply->error() != QNetworkReply::NoError) || !reply->isReadable())
    {
        LOG_ERR << reply->error();
        LOG_ERR << reply->errorString().toStdString();
        //TODO handle error string
        emit ctx->pending->error(QString());
        return;
    }
    
    QByteArray data = reply->readAll();

    LOG_DEBUG << "Reply data:" << QString(data).toStdString();

    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (!redirect.isEmpty()) {
        QNetworkReply* redirected_reply = get(ctx, redirect);
        Q_UNUSED(redirected_reply);
        return;
    }

    try {
        Q_ASSERT(!ctx->processors.empty());
        Processor proc = ctx->processors.front();
        ctx->processors.pop_front();
        
        QString localtion = reply->rawHeader("Location");
        
        if (!localtion.isEmpty())
        {
            data = localtion.toUtf8();
        }
        
        proc(ctx, data);
    }
    catch (std::exception& e) {
        LOG_ERR << "action %1 has error %2", ctx->id, e.what();
        emit ctx->pending->error(e.what());
    }

    if (ctx->processors.empty())
        emit ctx->pending->finished();
}

void gdocs::network_accessible_changed(int accessible) const
{
    LOG_DEBUG << "network_accessible_changed encountered:" << accessible;
}

void gdocs::proxy_authentication_required(const QNetworkProxy& proxy, QAuthenticator* authenticator) const
{
    Q_UNUSED(proxy);
    Q_UNUSED(authenticator);
    LOG_DEBUG << "proxy_authentication_required encountered:";
}

void gdocs::ssl_errors(QNetworkReply* reply, const QList<QSslError>& errors) const
{
    LOG_ERR << "ssl_errors encountered:";
    BOOST_FOREACH(const QSslError& err, errors) {
        LOG_ERR << err.errorString().toStdString();
    }
    ContextPtr ctx = reply->property(action_property_c).value<ContextPtr>();
    introspect(reply);
    introspect(ctx);
}


std::auto_ptr<gdocs_action> gdocs::create_action(const remote::group& obj)
{
    std::auto_ptr<context> ctx(new context(id_++, obj));
    ctx->filename = obj.type() + ".txt";
    ctx->http_headers["User-Agent"] = app_name_.toUtf8();
    ctx->http_headers["GData-Version"] = "3.0";

    return std::auto_ptr<gdocs_action>(new gdocs_action(ctx, this));
}

void gdocs::start(std::auto_ptr<context> ctx, std::auto_ptr<gdocs_action> act)
{
    QUrl url(login_uri_c);
    url.addQueryItem("Email",       login_);
    url.addQueryItem("Passwd",      password_);
    url.addQueryItem("accountType", "HOSTED_OR_GOOGLE");
    url.addQueryItem("source",      app_name_);
    url.addQueryItem("service",     "writely");
        
    ctx->pending = act; //context owns action
    //QNetworkAccessManager owns context
    QNetworkReply* auth_reply = get(ContextPtr(ctx.release()), url);
    Q_UNUSED(auth_reply);
}


QNetworkReply* gdocs::get(ContextPtr ctx, const QUrl& url)
{
    GDOCS_DEBUG << url.toString().toStdString();
    QNetworkRequest request(url);

    BOOST_FOREACH(const Headers::value_type& h, ctx->http_headers) {
        request.setRawHeader(h.first, h.second);
    }

    QList<QByteArray> l = request.rawHeaderList();
    QStringList sl;
    std::copy(l.begin(), l.end(), std::back_inserter(sl));
    
    LOG_DEBUG << sl.join("\n").toStdString();
    
    QNetworkReply* reply = manager_->get(request);
    //reply is now owning context
    reply->setProperty(action_property_c, qVariantFromValue(ctx));
    return reply;
}

void gdocs::process_auth(ContextPtr ctx, const QByteArray& data)
{
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);
        
    QString auth= QString(data).section("Auth=", 1).trimmed();
    ctx->http_headers["Authorization"] = QString("GoogleLogin auth=%1").arg(auth).toUtf8();

    //TODO handle error better
    if (auth.isEmpty())
        throw std::runtime_error("Auth is empty!");

    //using search by title
    QUrl url(base_uri_c.toString() + "?title="+ctx->filename);

    QNetworkReply* docs_reply = get(ctx, url);
    Q_UNUSED(docs_reply);
}

void gdocs::process_query(ContextPtr ctx, const QByteArray& data)
{
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);
    
    foreach(const document& d, gdocs_documents(data).documents()) {
        if (d.filename != ctx->filename) continue;
        
        ctx->doc = d;
        break;
    }
    
    if (!ctx->doc.id.isEmpty())
    {
        LOG_DEBUG << "Finded file: "<< ctx->doc.filename.toStdString();
        LOG_DEBUG << "Finded id: "  << ctx->doc.id.toStdString();
        LOG_DEBUG << "Finded src: " << ctx->doc.src.toStdString();
        emit ctx->pending->exists();
    }
    else
    {
        LOG_DEBUG << "Document not founded: " << ctx->filename.toStdString();
    }

    if (!ctx->processors.empty()){
        Processor proc = ctx->processors.front();
        ctx->processors.pop_front();
        proc(ctx, data);
    }
}

void gdocs::download_impl(ContextPtr ctx, const QByteArray& data)
{
    Q_UNUSED(data);
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);

    if (ctx->doc.id.isEmpty())
        throw std::runtime_error("file not found");
    
    QUrl url(download_uri_c);
    url.setQueryItems( QueryList()
        //by API 3.0 documentation we must do "docID", but it leads to redirect response. If it doesn't work use "id" key.
        //<< QueryArg("id",          ctx->doc.id)
        << QueryArg("docID",       ctx->doc.id)
        << QueryArg("exportFormat","txt" )
        << QueryArg("formatormat", "txt" )
    );

    QNetworkReply* download_reply = get(ctx, url);
    Q_UNUSED(download_reply);
}

void gdocs::upload_impl(ContextPtr ctx, const QByteArray& data)
{
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);

    QString upload = gdocs_documents(data).upload_url();
    LOG_DEBUG << "UPLOAD:"<<upload.toStdString();
    
    QUrl url(upload);
//     url.addQueryItem("convert", "false");
    
    LOG_DEBUG << "http POST: " << url.toString().toStdString();
    QNetworkRequest request(url);

    BOOST_FOREACH(const Headers::value_type& h, ctx->http_headers) {
        request.setRawHeader(h.first, h.second);
    }
    
    QByteArray content = remote::to_json(ctx->obj.save());
    
    LOG_DEBUG << "POST SIZE:"<< content.size();
    
    request.setRawHeader("Content-Type", "application/atom+xml");
    request.setRawHeader("X-Upload-Content-Type", "text/plain");
    request.setRawHeader("X-Upload-Content-Length", QString::number(content.size()).toUtf8());
    
    QByteArray meta = ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                      "<entry xmlns=\"http://www.w3.org/2005/Atom\" xmlns:docs=\"http://schemas.google.com/docs/2007\">"
                      "<title>" + ctx->filename + "</title>"
                      "</entry>").toUtf8();
    
    ctx->processors << bind(&gdocs::process_upload, this, _1, url, _2 );
//     ctx->processors << bind(&gdocs::process_upload, this, _1, url, _2 );
                      
    QNetworkReply* reply = manager_->post(request, meta);
    reply->setProperty(action_property_c, qVariantFromValue(ctx));
//     return reply;
    LOG_DEBUG << "posted";
    //TODO implement
}

void gdocs::process_download(ContextPtr ctx, const QByteArray& data)
{
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);
    emit ctx->pending->loaded(remote::from_json(data));
}

void gdocs::process_upload(ContextPtr ctx, const QUrl& u, const QByteArray& data)
{
    GDOCS_DEBUG << ctx->id;
    introspect(ctx);

    if (u.isEmpty())
        return;

    QUrl url( QUrl::fromEncoded(data));
    
    LOG_DEBUG << "http PUT: " << url.toString().toStdString();
//     LOG_DEBUG << "http PUT url: " << 
    QNetworkRequest request(url);

    BOOST_FOREACH(const Headers::value_type& h, ctx->http_headers) {
        request.setRawHeader(h.first, h.second);
    }
    
    QByteArray content = remote::to_json(ctx->obj.save());
    
    LOG_DEBUG << "PUT SIZE:"<< content.size();
    LOG_DEBUG << "PUT CONTENT:"<< content.constData();
    
    request.setRawHeader("Content-Type", "text/plain");
//     request.setRawHeader("Content-Range", QString("0-%1/%1").arg(content.size()).toUtf8());
//     Content-Range: bytes 0-524287/1073741824
    
    
    LOG_DEBUG << "1";
                      
    QNetworkReply* reply = manager_->put(request, content);
     reply->setProperty(action_property_c, qVariantFromValue(ctx));
    
    LOG_DEBUG << "2";
    
    
//     emit ctx->pending->saved();
    
     ctx->processors << boost::bind(&gdocs::process_upload, this, _1, QUrl(), _2);
}



