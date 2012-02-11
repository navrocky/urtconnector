
#include <QFileInfo>
#include <QDir>

#include "fileservice.h"

struct fileaction : remote::action {
    
    enum Action {read, write, check};
    
    fileaction(const QString& filename, Action act, const QByteArray& data = QByteArray())
        : filename_(filename)
        , action_(act)
        , data_(data)
    {
    }
    
    virtual void start()
    {
        try {
            QFile file(filename_);
            if (!file.open(QIODevice::ReadWrite))
                throw std::runtime_error(std::string("Can't open file ") + filename_.toStdString());

            emit exists();
            
            switch (action_) {
            case read:
                data_ = file.readAll();
                emit loaded(remote::from_json(data_));
                break;
            case write:
                file.resize(0);
                file.write(data_);
                emit saved();
            case check:
                break;
            };
        }
        catch (std::exception& e) {
            emit error(QString(e.what()));
        }
       
        emit finished();
        deleteLater();
    }
    
    virtual void abort(){}
        
    QString filename_;
    Action action_;
    QByteArray data_;
};


filestorage::filestorage(const QString& path, QObject* parent)
    : QObject(parent)
    , path_(path)
{
    if (!QFileInfo(path_).exists())
    {
        if(!QDir().mkpath(path_))
            throw std::runtime_error(std::string("Can't create directory ") + path_.toStdString());
    }
    
    
}

filestorage::~filestorage()
{
}

QString mkname(const QString& path, const QString& type) {
    return path + "/" + QString("%1.txt").arg(type);
}

remote::action* filestorage::get(const QString& type)
{
    return new fileaction(mkname(path_, type), fileaction::read);
}

remote::action* filestorage::put(const remote::group& gr)
{
    return new fileaction(mkname(path_, gr.type()), fileaction::write, remote::to_json(gr.save()));
}

remote::action* filestorage::check(const QString& type)
{
    return new fileaction(mkname(path_, type), fileaction::check);
}







