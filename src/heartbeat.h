#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include <QObject>

class QNetworkAccessManager;

class heartbeat : public QObject
{
public:
    heartbeat(QObject* parent = 0);

    void exec();

private:
    QNetworkAccessManager* manager_;
};

#endif // HEARTBEAT_H
