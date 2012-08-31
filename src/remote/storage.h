
#ifndef URT_STORAGE_H
#define URT_STORAGE_H

#include <stdexcept>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QObject>
#include <QIcon>
#include <QVariantMap>
//#include <QUuid>

class QSettings;
class QWidget;

namespace remote {

class group;

/*! \brief Pending action interface */
class action: public QObject {
    Q_OBJECT
public:
    
    enum status_e
    {
        None    = 0,
        Loaded  = 1 << 0,
        Saved   = 1 << 1,
        Exists  = 1 << 2,
        Error   = 1 << 3, 
        Finished= 1 << 4,
        Aborted = 1 << 5,
    };
    
    typedef int Status;
    
    /*! \brief Starts execution of the action.
     * \warning action can be synchronous, therefor don't use this object directly
     * after calling this method, because of it can be even deleted.
     * While this methos call all signals(including finished) may emiting
    */
    virtual void start() = 0;

    virtual void abort() = 0;
    
    virtual Status status() const = 0;
    
Q_SIGNALS:
    /*! \brief Group successfuly downloaded from storoge */
    void loaded(const remote::group& gr);

    /*! \brief Group successfuly uploaded to storage */
    void saved();

    /*! \brief Group exists on storage */
    void exists();

    /*! \brief Error occured while action executing */
    void error(const QString& err);

    /*! \brief Action compleated. May be error were occured */
    void finished();
};


/*! \brief Storage interface
 * Represents instance of any service(account)
 */
struct storage {

    virtual action* get(const QString& type) = 0;
    virtual action* put(const group& gr) = 0;
    virtual action* check(const QString& type) = 0;

};

/*! \brief base class for any service(Google docs, DropBox, etc)
 * 
 */
class service {
public:

    typedef boost::shared_ptr<storage> Storage;

    service(const QString& c, const QString& d, const QIcon& i = QIcon())
        : caption_(c), desc_(d), icon_(i)
    {}
    
    const QString& caption() const { return caption_; };
    const QString& description() const {  return desc_; };
    const QIcon& icon() const {  return icon_; };

    Storage create(const boost::shared_ptr<QSettings>& settings) const {
        return do_create(settings);
    }

    virtual std::auto_ptr<QVariantMap> configure(const QVariantMap& settings = QVariantMap()) const = 0;
    
protected:
    virtual Storage do_create(const boost::shared_ptr<QSettings>& settings) const = 0;
    
private:
    QString caption_;
    QString desc_;
    QIcon icon_;
};

// class storage_manager {
// 
//     boost::shared_ptr<storage> create(boost::shared_ptr<const service>);
//     
//     void destroy(boost::shared_ptr<storage>);
//     
//     std::list<boost::shared_ptr<storage> > list() const;
// 
//     QWidget* config(boost::shared_ptr<storage>);
// 
//     boost::shared_ptr<const service> get_service(boost::shared_ptr<storage>) const;
// 
//     std::list<boost::shared_ptr<const service> > services() const;
//     
// };

} // namespace remote

#endif
