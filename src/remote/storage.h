
#ifndef URT_STORAGE_H
#define URT_STORAGE_H

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

} // namespace remote

#endif
