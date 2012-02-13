
#ifndef URT_STORAGE_H
#define URT_STORAGE_H

#include <stdexcept>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include <QObject>
#include <QVariantMap>

class QSettings;
class QWidget;

namespace remote {

class group;

/*! \brief Pending action interface */
class action: public QObject {
    Q_OBJECT
public:
    /*! \brief Starts execution of the action.
     * \warning action can be synchronous, therefor don't use this object directly
     * after calling this method, because of it can be even deleted.
     * While this methos call all signals(including finished) may emiting
    */
    virtual void start() = 0;

    virtual void abort() = 0;
    
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


    void set_name(const QString& name) {name_ = name;};
    const QString& name() const { return name_; };

    virtual action* get(const QString& type) = 0;
    virtual action* put(const group& gr) = 0;
    virtual action* check(const QString& type) = 0;

private:
    QString name_;
};

/*! \brief base class for any service(Google docs, DropBox, etc)
 * 
 */
class service {
public:

    typedef boost::shared_ptr<storage> Storage;

    service(const QString& c, const QString& d)
        : caption_(c), desc_(d)
    {}
    
    const QString& caption() const { return caption_; };
    const QString& description() const {  return desc_; };

    Storage create(const boost::shared_ptr<QSettings>& settings) const {
        return do_create(settings);
    }

    virtual QVariantMap configure(const QVariantMap& settings = QVariantMap()) const = 0;
    
protected:
    virtual Storage do_create(const boost::shared_ptr<QSettings>& settings) const = 0;
    
private:
    QString caption_;
    QString desc_;
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
