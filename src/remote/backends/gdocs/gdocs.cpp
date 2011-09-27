
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

#include "gdocs.h"

SYSLOG_MODULE(gdocs);

typedef qt_signal_wrapper qsw;

const QUrl login_uri_c = QString("https://www.google.com/accounts/ClientLogin");

gdocs::gdocs(const QString& login, const QString& password, const QString& app_name, QObject* parent)
    : QObject(parent)
    , auth_(false)
    , manager_(new QNetworkAccessManager(this))
{
    generic_header_["User-Agent"]    = app_name.toUtf8();
    generic_header_["GData-Version"] = "2.0";
 
    connect(manager_, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)), this, SLOT(ssl_errors(QNetworkReply*,QList<QSslError>)));
    
    QStringList auth_data;
    auth_data  << QString("Email=%1").arg(login)
               << QString("Passwd=%1").arg(password)
               << QString("accountType=%1").arg("HOSTED_OR_GOOGLE")
               << QString("source=%1").arg(app_name)
               << QString("service=%1").arg("writely");

    QNetworkRequest auth_request( login_uri_c );
    fill_header(auth_request);
    
    std::cerr << "1" <<std::endl;
    QNetworkReply* auth_reply = manager_->post(auth_request, auth_data.join("&").toUtf8());
    std::cerr << "2" <<std::endl;
    connect(auth_reply, SIGNAL(finished()), this, SLOT(auth_finished()));
    std::cerr << "3" <<std::endl;
    connect(auth_reply, SIGNAL(error(QNetworkReply::NetworkError)), 
        new qsw(this, boost::bind( &gdocs::error, this, "Auth error" )), SLOT( activated() ) );
    
}

gdocs::~gdocs()
{}

void gdocs::auth_finished()
{
    QNetworkReply* reply = static_cast<QNetworkReply*>(sender());
    
    std::cerr << "auth_finished"<<std::endl;
    if( reply->error() != QNetworkReply::NoError ) {
        std::cerr << "auth_finished error!"<<std::endl;
        return;
    }
    
    QString all = reply->readAll();
    QString auth= all.section("Auth=", 1).trimmed();
    generic_header_["Authorization"] = QString("GoogleLogin auth=%1").arg(auth).toUtf8();
    auth_ = true;
    
    std::cerr<<"auth ok"<<std::endl;
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



