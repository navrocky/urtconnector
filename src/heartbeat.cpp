#include "heartbeat.h"

#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include "app_options.h"

heartbeat::heartbeat(QObject *parent)
    : QObject(parent)
    , manager_(new QNetworkAccessManager(this))
{
}

void heartbeat::exec()
{
    app_settings as;
    QNetworkRequest request(QUrl(QString("http://urtconnector.appspot.com/heartbeat?id=%1").arg(as.install_id())));
    manager_->get(request);
}
