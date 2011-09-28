
#ifndef URT_GDOCS_H
#define URT_GDOCS_H

#include <stdexcept>

#include <map>
#include <vector>

#include <QObject>


class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QSslError;

class QUrl;

class request_context;

class gdocs: public QObject{
Q_OBJECT
public:
    
    explicit gdocs( const QString& login, const QString& password, const QString& app_name, QObject* parent = 0 );
    virtual ~gdocs();
    
public Q_SLOTS:
    bool parse_auth(const QString& data);

    void load(const QString& filename);
    
    
private Q_SLOTS:
    void ssl_errors (QNetworkReply * reply, const QList<QSslError> & errors);
    void finished(QNetworkReply * reply);
    
private:
    void fill_header(QNetworkRequest& request);
    QNetworkReply* get(const QUrl& url, const request_context& ctx);
    
Q_SIGNALS:
    void error(const QString& str);

private:

    friend class request_context;
    enum Action {
        None        = 0,
        Auth        = 1 << 0,
        Query       = 1 << 1,
        Download    = 1 << 2
    };

    typedef int ActionMask;

    ActionMask state_;
    
private:
    bool auth_;
    QNetworkAccessManager* manager_;
    typedef std::map<QByteArray, QByteArray> Headers;
    Headers generic_header_;

    std::vector<QString> downloads;
    
};


#include <QFile>
#include <QDomDocument>

struct document{
    QString id;
    QString src;
    QString filename;
};

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
