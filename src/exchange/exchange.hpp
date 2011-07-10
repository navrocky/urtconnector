#ifndef _EXCHANGE_HPP
#define _EXCHANGE_HPP

#include <QHttp>
#include <QBuffer>
#include <QWidget>

#define HOST "pztrn.ru"
#define PORT 20080
#define HOSTPORT "pztrn.ru:20080"
#define URL "/sectest/index.page"

class exchange
{
    public:
        static exchange *GetInstance()
        {
            if (_instance == NULL)
            {
                return new exchange;
            }
            return _instance;
        }
        void NewRequest(QString boundary);
        void AddVar(QString name, QString value);
        void Request();
        bool Free() { return _free; };
        QBuffer &getBuf();
    private:
        exchange();
        ~exchange();

        QBuffer _buf;
        QHttp _http;
        QHttpRequestHeader _header;
        QByteArray _ba;
        QString _boundary;
        bool _free;

        static exchange* _instance;
};

#endif
