
#include <QSettings>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>

#include "common/json_tools.h"
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
                emit loaded(from_json(data_));
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


filestorage::filestorage(const QString& location, QObject* parent)
    : QObject(parent)
    , location_(location)
{
    if (!QFileInfo(location_).exists())
    {
        if(!QDir().mkpath(location_))
            throw std::runtime_error(std::string("Can't create directory ") + location_.toStdString());
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
    return new fileaction(mkname(location_, type), fileaction::read);
}

remote::action* filestorage::put(const remote::group& gr)
{
    return new fileaction(mkname(location_, gr.type()), fileaction::write, to_json(gr.save()));
}

remote::action* filestorage::check(const QString& type)
{
    return new fileaction(mkname(location_, type), fileaction::check);
}




remote::service::Storage fileservice::do_create(const boost::shared_ptr<QSettings>& settings) const
{
    if (!settings->contains("location"))
        throw std::runtime_error("settings not valid");
    return service::Storage(new filestorage(settings->value("location").toString()));
}


QVariantMap fileservice::configure(const QVariantMap& settings) const
{
    QVariantMap ret(settings);

    QString location = ret["location"].toString();

    location = QFileDialog::getExistingDirectory(
        0,
        QObject::tr("Select location for fileservice"),
        location);

    if (!location.isNull()) {
        ret["location"] = location;
    }

    return ret;
}







