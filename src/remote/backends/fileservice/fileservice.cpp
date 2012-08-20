
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
        , status_(None)
    {
    }

    virtual void start()
    {
        try {
            QFile file(filename_);
            if (!file.open(QIODevice::ReadWrite))
                throw std::runtime_error(std::string("Can't open file ") + filename_.toStdString());
            
            status_ |= Exists;
            emit exists();
            
            switch (action_) {
            case read:
                data_ = file.readAll();
                status_ |= Loaded;
                std::cerr << "LOADED: " << data_.constData() << std::endl;
                emit loaded(from_json(data_));
                break;
            case write:
                file.resize(0);
                file.write(data_);
                status_ |= Saved;
                emit saved();
            case check:
                break;
            };
        }
        catch (std::exception& e) {
            status_ |= Error;
            emit error(QString(e.what()));
        }
       
        status_ |= Finished;
        emit finished();
        deleteLater();
    }
    
    virtual void abort() {
        status_ |= Aborted;
    }
    
    virtual Status status() const {
        return status_;
    }
        
    QString filename_;
    Action action_;
    QByteArray data_;
    Status status_;
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


fileservice::fileservice()
    : service("fileservice", "Synchronization with files", QIcon::fromTheme("folder", QIcon::fromTheme("folder-open")))
{
}


remote::service::Storage fileservice::do_create(const boost::shared_ptr<QSettings>& settings) const
{
    if (!settings->contains("location"))
        throw std::runtime_error("settings not valid");
    return service::Storage(new filestorage(settings->value("location").toString()));
}


std::auto_ptr<QVariantMap> fileservice::configure(const QVariantMap& settings) const
{
    QVariantMap ret(settings);

    QString location = ret["location"].toString();

    location = QFileDialog::getExistingDirectory(
        0,
        QObject::tr("Select location for fileservice"),
        location);

    if (location.isNull()) {
        return std::auto_ptr<QVariantMap>();
    }
    
    ret["location"] = location;
    
    return std::auto_ptr<QVariantMap>(new QVariantMap(ret));
}








