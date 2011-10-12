
#ifndef URT_GDOCS_H
#define URT_GDOCS_H

#include <stdexcept>

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "../../storage.h"


class QAuthenticator;
class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QSslError;

class QUrl;

class request_context;

class context;
class gdocs_action;
typedef boost::shared_ptr<context> ContextPtr;

struct document {
    QString id;
    QString src;
    QString filename;
};

class gdocs: public QObject, remote::storage {
Q_OBJECT
public:
    
    explicit gdocs( const QString& login, const QString& password, const QString& app_name, QObject* parent = 0 );
    virtual ~gdocs();


    virtual remote::action* get(const QString& type);
    virtual remote::action* put(const remote::object& obj);
    virtual remote::action* check(const QString& type);

private Q_SLOTS:
    void authentication_required(QNetworkReply * reply, QAuthenticator * authenticator) const;
    void finished(QNetworkReply * reply);
    void network_accessible_changed(int accessible) const;
    void proxy_authentication_required (const QNetworkProxy & proxy, QAuthenticator * authenticator) const;
    void ssl_errors(QNetworkReply * reply, const QList<QSslError> & errors) const;
    
private:
    friend class gdocs_action;
    std::auto_ptr<gdocs_action> create_action(const remote::object& obj);
    void start(std::auto_ptr<context> ctx, std::auto_ptr<gdocs_action> act);
    QNetworkReply* get(ContextPtr ctx, const QUrl& url);

    void process_auth(ContextPtr ctx, const QByteArray& data);
    void process_query(ContextPtr ctx, const QByteArray& data);
    void process_download(ContextPtr ctx, const QByteArray& data);
    void process_upload(ContextPtr ctx, const QByteArray& data);

    void download_impl(ContextPtr ctx, const QByteArray& data);
    void upload_impl(ContextPtr ctx, const QByteArray& data);
   
private:
    QNetworkAccessManager* manager_;

    QString login_;
    QString password_;
    QString app_name_;
    int id_;
};


#include <QFile>
#include <QDomDocument>



class gdocs_documents {
public:
    gdocs_documents(const QByteArray& xml_data)
    {
        QString err;
        int l;
        int c;
        if ( !dom_.setContent(xml_data, false, &err, &l, &c) )
        {
            throw std::runtime_error(
                QString("Error in parsing xml: %1. line: %2, column %3")
                    .arg(err).arg(l).arg(c).toStdString() );
        }
    }

    std::list<document> documents() const {
        std::list<document> ret;
        QDomNodeList entries = dom_.elementsByTagName("entry");

        for (uint i = 0; i < entries.size(); ++ i)
        {
            document d;

            //api 2
            //QString tmp = "http://docs.google.com/feeds/documents/private/full/document%3A";
            
            //api3
            QString tmp = "https://docs.google.com/feeds/id/document%3A";

            d.id = entries.at(i).firstChildElement("id").firstChild().nodeValue();
            d.filename = entries.at(i).firstChildElement("title").firstChild().nodeValue();

            d.id.replace(tmp, "");

            d.src = entries.at(i).firstChildElement("content").attribute("src");
            
            ret.push_back(d);
        }

        return ret;
    }

private:
    QDomDocument dom_;
};


#endif
