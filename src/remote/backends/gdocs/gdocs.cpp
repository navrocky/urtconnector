
#include <boost/foreach.hpp>
#include <boost/bind.hpp>

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>

#include <QString>
#include <QStringList>

#include <iostream>

#include <cl/syslog/syslog.h>

#include "common/tools.h"

#include "gdocs.h"

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

typedef boost::function<void (ActionPtr, const QByteArray&)> Processor;

struct action {
    int id;
    QString filename;
    
    QString auth;
    document doc;
    Headers http_headers;
    QList<Processor> processors;
    std::auto_ptr<pending_action> pending;
};

Q_DECLARE_METATYPE(ActionPtr);

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
}

void introspect(const document& doc) {
    LOG_HARD << "introspecting document:";
    LOG_HARD << "id: "      << doc.id.toStdString();
    LOG_HARD << "src: "     << doc.src.toStdString();
    LOG_HARD << "filename: "<< doc.filename.toStdString();
}

void introspect(const ActionPtr& act) {
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

pending_action* gdocs::load(const QString& filename)
{
    LOG_DEBUG << "load request: " << filename.toStdString();

    ActionPtr act = new_action(filename);

    act->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2)
        << boost::bind(&gdocs::download_impl, this, _1, _2)
        << boost::bind(&gdocs::process_download, this, _1, _2);

    QUrl url(login_uri_c);
    url.addQueryItem("Email",       login_);
    url.addQueryItem("Passwd",      password_);
    url.addQueryItem("accountType", "HOSTED_OR_GOOGLE");
    url.addQueryItem("source",      app_name_);
    url.addQueryItem("service",     "writely");
        
    QNetworkReply* auth_reply = get(act, url);
    Q_UNUSED(auth_reply);
}

pending_action* gdocs::save(const QString& filename, const QByteArray& data)
{
    LOG_DEBUG << "save request: " << filename.toStdString();
    ActionPtr act = new_action(filename);
    
    act->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2)
        << boost::bind(&gdocs::upload_impl, this, _1, _2)
        << boost::bind(&gdocs::process_upload, this, _1, _2);

    QUrl url(login_uri_c);
    url.addQueryItem("Email",       login_);
    url.addQueryItem("Passwd",      password_);
    url.addQueryItem("accountType", "HOSTED_OR_GOOGLE");
    url.addQueryItem("source",      app_name_);
    url.addQueryItem("service",     "writely");

    QNetworkReply* auth_reply = get(act, url);
    Q_UNUSED(auth_reply);
}

pending_action* gdocs::check(const QString& filename)
{
    LOG_DEBUG << "check request: " << filename.toStdString();
    ActionPtr act = new_action(filename);

    act->processors
        << boost::bind(&gdocs::process_auth, this, _1, _2)
        << boost::bind(&gdocs::process_query, this, _1, _2);

    QUrl url(login_uri_c);
    url.addQueryItem("Email",       login_);
    url.addQueryItem("Passwd",      password_);
    url.addQueryItem("accountType", "HOSTED_OR_GOOGLE");
    url.addQueryItem("source",      app_name_);
    url.addQueryItem("service",     "writely");

    QNetworkReply* auth_reply = get(act, url);
    Q_UNUSED(auth_reply);    
}

void gdocs::authentication_required(QNetworkReply* reply, QAuthenticator* authenticator) const
{
    LOG_DEBUG << "authentication_required encountered:";
    ActionPtr act = reply->property(action_property_c).value<ActionPtr>();
    introspect(reply);
    introspect(act);
}

void gdocs::finished(QNetworkReply* reply)
{
    ActionPtr act = reply->property(action_property_c).value<ActionPtr>();
    LOG_DEBUG << "Reply reply: " << act->id;
    introspect(act);
    introspect(reply);
    
    if ((reply->error() != QNetworkReply::NoError) || !reply->isReadable())
    {
        LOG_ERR << reply->error();
        LOG_ERR << reply->errorString().toStdString();
        //TODO handle error string
        act->pending->error(QString());
        return;
    }
    
    QByteArray data = reply->readAll();

    LOG_DEBUG << "Reply data:" << QString(data).toStdString();

    QUrl redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toUrl();

    if (!redirect.isEmpty()) {
        QNetworkReply* redirected_reply = get(act, redirect);
        Q_UNUSED(redirected_reply);
        return;
    }

    try {
        Q_ASSERT(act->processors.empty());
        Processor proc = act->processors.front();
        act->processors.pop_front();
        proc(act, data);
    }
    catch (std::exception& e) {
        LOG_ERR << "action %1 has error %2", act->id, e.what();
        act->pending->error(e.what());
    }
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
    ActionPtr act = reply->property(action_property_c).value<ActionPtr>();
    introspect(reply);
    introspect(act);
}

void gdocs::process_auth(ActionPtr act, const QByteArray& data)
{
    LOG_DEBUG << "process_auth: " << act->id;
    introspect(act);
        
    QString auth= QString(data).section("Auth=", 1).trimmed();
    act->http_headers["Authorization"] = QString("GoogleLogin auth=%1").arg(auth).toUtf8();

    //TODO handle error better
    if (auth.isEmpty())
        throw std::runtime_error("Auth is empty!");

    //using search by title
    QUrl url(base_uri_c.toString() + "?title="+act->filename);

    QNetworkReply* docs_reply = get(act, url);
    Q_UNUSED(docs_reply);
}

void gdocs::process_query(ActionPtr act, const QByteArray& data)
{
    LOG_DEBUG << "process_query: " << act->id;
    introspect(act);
    
    foreach(const document& d, gdocs_documents(data).documents()) {
        if (d.filename != act->filename) continue;
        
        act->doc = d;
        break;
    }

    if (!act->doc.id.isEmpty())
    {
        LOG_DEBUG << "Finded file: "<< act->doc.filename.toStdString();
        LOG_DEBUG << "Finded id: "  << act->doc.id.toStdString();
        LOG_DEBUG << "Finded src: " << act->doc.src.toStdString();
        act->pending->exists();
    }
    else
    {
        LOG_DEBUG << "Document not founded: " << act->filename.toStdString();
    }

    if (!act->processors.empty()){
        Processor proc = act->processors.front();
        act->processors.pop_front();
        proc(act, QByteArray());
    }
}

void gdocs::download_impl(ActionPtr act, const QByteArray& data)
{
    Q_UNUSED(data);
    LOG_DEBUG << "download_impl: " << act->id;
    introspect(act);

    if (act->doc.id.isEmpty())
        throw std::runtime_error("file not found");
    
    QUrl url(download_uri_c);
    url.setQueryItems( QueryList()
        //by API 3.0 documentation we must do "docID", but it leads to redirect response. If it doesn't work use "id" key.
        //<< QueryArg("id",          act->doc.id)
        << QueryArg("docID",       act->doc.id)
        << QueryArg("exportFormat","txt" )
        << QueryArg("formatormat", "txt" )
    );

    QNetworkReply* download_reply = get(act, url);
    Q_UNUSED(download_reply);
}

void gdocs::upload_impl(ActionPtr act, const QByteArray& data)
{
    LOG_DEBUG << "create_impl: " << act->id;
    introspect(act);

    //TODO implement
}

void gdocs::process_download(ActionPtr act, const QByteArray& data)
{
    LOG_DEBUG << "process_download: " << act->id;
    introspect(act);
    act->pending->loaded(data);
}

void gdocs::process_upload(ActionPtr act, const QByteArray& data)
{
    LOG_DEBUG << "process_upload: " << act->id;
    introspect(act);
    act->pending->saved();
}


ActionPtr gdocs::new_action(const QString& filename)
{
    ActionPtr act(new action);
    act->id = id_++;
    act->filename = filename;
    act->http_headers["User-Agent"] = app_name_.toUtf8();
    act->http_headers["GData-Version"] = "3.0";
    act->pending.reset(new pending_action(0));
   
    return act;
}

QNetworkReply* gdocs::get(ActionPtr act, const QUrl& url)
{
    QNetworkRequest request(url);

    BOOST_FOREACH(const Headers::value_type& h, act->http_headers) {
        request.setRawHeader(h.first, h.second);
    }

    QNetworkReply* reply = manager_->get(request);
    reply->setProperty(action_property_c, qVariantFromValue(act));
    return reply;
}






