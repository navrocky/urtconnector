
#ifndef URT_STORAGE_H
#define URT_STORAGE_H

#include <boost/shared_ptr.hpp>

#include <QObject>


namespace remote {

class object;


class action: public QObject {
    Q_OBJECT
public:
    virtual void start() = 0;

Q_SIGNALS:
    void loaded(const object& obj);
    void saved();
    void exists();
    void error(const QString& err);
};


/*! backend interface */

class storage {

public:
    virtual action* get(const QString& type) = 0;
    virtual action* put(const object& obj) = 0;
    virtual action* check(const QString& type) = 0;
};




class service {

//     service(const QString& caption, const QString& desc, const qsettings)
    
    const QString& caption() const;
    const QString& description() const;
};

class storage_manager {

    boost::shared_ptr<storage> create(boost::shared_ptr<const service>);
    
    void destroy(boost::shared_ptr<storage>);
    
    std::list<boost::shared_ptr<storage> > list() const;

    QWidget* config(boost::shared_ptr<storage>);

    boost::shared_ptr<const service> get_service(boost::shared_ptr<storage>) const;

    std::list<boost::shared_ptr<const service> > services() const;
    
};

} // namespace remote

#endif
