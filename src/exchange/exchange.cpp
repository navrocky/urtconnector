#include "exchange.hpp"
#include <QEventLoop>

void exchange::NewRequest(QString boundary)
{
    if (!_free)
    {
        return;
    }
    _boundary = boundary;
    QStringList keys = _header.keys();
    int count = keys.count();
    for (int i=0; i<count; ++i)
    {
        _header.removeAllValues(keys[i]);
    }
    _header.setRequest("POST",URL);
    _header.setValue("Host", HOSTPORT);
    _header.setValue("Content-Type", "multipart/form-data; boundary="+boundary);
    _ba.clear();
}
void exchange::AddVar(QString name, QString value)
{
    _ba += "--";
    _ba += _boundary;
    _ba += "\r\n";
    _ba += "Content-Disposition: form-data; name=\"";
    _ba += name;
    _ba += "\"\r\n\r\n";
    _ba += value;
    _ba += "\r\n--";
    _ba += _boundary;
    _ba += "\r\n";
}
void exchange::Request()
{
    if (!_free)
    {
        return;
    }
    _free = false;
    _buf.reset();
    _header.setValue("Content-Length", QString(_ba.length()));

    QEventLoop loop;
    loop.connect(&_http, SIGNAL(done(bool)), SLOT(quit()));
    _http.request(_header, _ba, &_buf);
    loop.exec();
    _free = true;
}
QBuffer& exchange::getBuf()
{
    _buf.seek(0);
    return _buf;
}
exchange::exchange()
{
    _buf.open(QIODevice::ReadWrite);
    _instance = this;
    _http.setHost(HOST,PORT);
//    connect(&_http, SIGNAL(done(bool)), this, SLOT(http_done(bool)));
    _free = true;
}
exchange::~exchange()
{
    _instance = NULL;
}
