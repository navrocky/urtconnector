
#ifndef URT_GDOC_H
#define URT_GDOC_H

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <iostream>

class gdoc: public QObject{
Q_OBJECT
public:
    explicit gdoc ( QObject* parent = 0 ){};
    virtual ~gdoc() {};
    
    void set_reply(QNetworkReply* rp) { rp_=rp;}
    
public Q_SLOTS:
    void finished(){
        QString all = rp_->readAll();
        std::cerr<<"All:"<<all.toStdString()<<std::endl;
        auth = all.section("Auth=", 1);
        std::cerr<<"MyAuth:"<<auth.toStdString()<<std::endl;

        auth.resize(auth.size()-1);
        std::cerr<<"MyAuth2:"<<auth.toStdString()<<std::endl;
        
        QString str("http://docs.google.com/feeds");
        str += "/documents/private/full";
        
        

        
        
        QNetworkAccessManager* nm = rp_->manager();
        
/*        QNetworkRequest doclist = QNetworkRequest( QUrl("https://docs.google.com/feeds/documents/private/full") );
        
        doclist.setRawHeader("User-Agent", "japp");
        doclist.setRawHeader("GData-Version", "2.0");

        QString aaa = QString("GoogleLogin auth=%1").arg(auth);

        doclist.setRawHeader("Authorization", aaa.toUtf8());
        
        rp_ = nm->get(doclist);*/

        QNetworkRequest upload = QNetworkRequest( QUrl("https://docs.google.com/feeds/documents/private/full") );
        upload.setRawHeader("User-Agent", "japp");
        upload.setRawHeader("GData-Version", "2.0");

        QString aaa = QString("GoogleLogin auth=%1").arg(auth);

        upload.setRawHeader("Authorization", aaa.toUtf8());
        
        
        upload.setRawHeader("Content-Type", "text/plain");
        upload.setRawHeader("Slug", "test.txt");
        


        rp_ = nm->post(upload, "yyyyyyyeeeeeeehhhhhhaaaaaa1");

        connect(rp_, SIGNAL(finished()), this, SLOT(doclist_finished()));
         
    }
    
    
    void doclist_finished(){
        std::cerr<<"err:"<<rp_->error()<<std::endl;
        QString all = rp_->readAll();
        std::cerr<<"All doclist:"<<all.toStdString()<<std::endl;
        
    }
    
public:
    QString auth;
    
private:
    QNetworkReply* rp_;
    
};

#endif

