
#ifndef URT_STORAGE_H
#define URT_STORAGE_H

class QString;

namespace remote {

class object;

/*! backend interface */
struct storage {
    virtual object get( const QString& type ) = 0;
    virtual void put( const object& obj ) = 0;
};

} // namespace remote

#endif
