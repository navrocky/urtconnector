
#ifndef URT_GDOCS_H
#define URT_GDOCS_H

#include <map>

#include <QObject>


class QNetworkAccessManager;
class QNetworkReply;
class QNetworkRequest;
class QSslError;

class gdocs: public QObject{
Q_OBJECT
public:
    explicit gdocs( const QString& login, const QString& password, const QString& app_name, QObject* parent = 0 );
    virtual ~gdocs();
    
public Q_SLOTS:
    void auth_finished();
//     void finished(){
//         QString all = rp_->readAll();
//         std::cerr<<"All:"<<all.toStdString()<<std::endl;
//         auth = all.section("Auth=", 1);
//         std::cerr<<"MyAuth:"<<auth.toStdString()<<std::endl;
// 
//         auth.resize(auth.size()-1);
//         std::cerr<<"MyAuth2:"<<auth.toStdString()<<std::endl;
//         
//         QString str("http://docs.google.com/feeds");
//         str += "/documents/private/full";
//         
//         
// 
//         
//         
//         QNetworkAccessManager* nm = rp_->manager();
//         
// /*        QNetworkRequest doclist = QNetworkRequest( QUrl("https://docs.google.com/feeds/documents/private/full") );
//         
//         doclist.setRawHeader("User-Agent", "japp");
//         doclist.setRawHeader("GData-Version", "2.0");
// 
//         QString aaa = QString("GoogleLogin auth=%1").arg(auth);
// 
//         doclist.setRawHeader("Authorization", aaa.toUtf8());
//         
//         rp_ = nm->get(doclist);*/
// 
//         QNetworkRequest upload = QNetworkRequest( QUrl("https://docs.google.com/feeds/documents/private/full") );
//         upload.setRawHeader("User-Agent", "japp");
//         upload.setRawHeader("GData-Version", "2.0");
// 
//         QString aaa = QString("GoogleLogin auth=%1").arg(auth);
// 
//         upload.setRawHeader("Authorization", aaa.toUtf8());
//         
//         
//         upload.setRawHeader("Content-Type", "text/plain");
//         upload.setRawHeader("Slug", "test.txt");
//         
// 
// 
//         rp_ = nm->post(upload, "yyyyyyyeeeeeeehhhhhhaaaaaa1");
// 
//         connect(rp_, SIGNAL(finished()), this, SLOT(doclist_finished()));
//          
//     }
//     
//     
//     void doclist_finished(){
//         std::cerr<<"err:"<<rp_->error()<<std::endl;
//         QString all = rp_->readAll();
//         std::cerr<<"All doclist:"<<all.toStdString()<<std::endl;
//         
//     }
    
    
private Q_SLOTS:
    void ssl_errors (QNetworkReply * reply, const QList<QSslError> & errors);
    
private:
    void fill_header(QNetworkRequest& request);
    
Q_SIGNALS:
    void error(const QString& str);
    
   
private:
    bool auth_;
    QNetworkAccessManager* manager_;
    typedef std::map<QByteArray, QByteArray> Headers;
    Headers generic_header_;
    
};


#endif
