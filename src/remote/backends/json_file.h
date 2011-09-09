
#ifndef URT_JSON_FILE_H
#define URT_JSON_FILE_H

#include "remote/storage.h"

namespace remote {

    /*! simple test backend*/
    class json_file_storage: public storage {
    public:

        json_file_storage(const QString& filename): filename_(filename) {}

        virtual object get( const QString& type );
        virtual void put( const object& obj );

    private:
        QString filename_;
    };

} // namespace remote


#endif