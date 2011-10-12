
#ifndef URT_GDOCS_H
#define URT_GDOCS_H

#include <stdexcept>

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include <QObject>


class QAuthenticator;
class QNetworkAccessManager;
class QNetworkProxy;
class QNetworkReply;
class QNetworkRequest;
class QSslError;

class QUrl;

class request_context;

class action;
typedef boost::shared_ptr<action> ActionPtr;

class pending_action: public QObject {
    Q_OBJECT
public:
    virtual ~pending_action(){};

Q_SIGNALS:
    void loaded(const QByteArray& data);
    void saved();
    void exists();
    void error(const QString& err);
    
private:
    friend class gdocs;
    pending_action(QObject* parent){};    
};

struct document {
    QString id;
    QString src;
    QString filename;
};

class gdocs: public QObject{
Q_OBJECT
public:
    
    explicit gdocs( const QString& login, const QString& password, const QString& app_name, QObject* parent = 0 );
    virtual ~gdocs();


public Q_SLOTS:
    pending_action* load(const QString& filename);
    pending_action* save(const QString& filename, const QByteArray& data);
    pending_action* check(const QString& filename);
    
private Q_SLOTS:
    void authentication_required(QNetworkReply * reply, QAuthenticator * authenticator) const;
    void finished(QNetworkReply * reply);
    void network_accessible_changed(int accessible) const;
    void proxy_authentication_required (const QNetworkProxy & proxy, QAuthenticator * authenticator) const;
    void ssl_errors(QNetworkReply * reply, const QList<QSslError> & errors) const;
    
private:
    ActionPtr new_action(const QString& filename);
    QNetworkReply* get(ActionPtr action, const QUrl& url);

    void process_auth(ActionPtr action, const QByteArray& data);
    void process_query(ActionPtr action, const QByteArray& data);
    void process_download(ActionPtr action, const QByteArray& data);
    void process_upload(ActionPtr action, const QByteArray& data);

    void download_impl(ActionPtr action, const QByteArray& data);
    void upload_impl(ActionPtr action, const QByteArray& data);

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
