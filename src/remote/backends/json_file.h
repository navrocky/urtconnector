
#ifndef URT_JSON_FILE_H
#define URT_JSON_FILE_H

#include "remote/storage.h"

namespace remote {

    /*! simple test backend*/
    /*class json_file_storage: public storage {
    public:

        json_file_storage(const QString& filename): filename_(filename) {}

        virtual action* get( const QString& type );
        virtual action* put( const group& obj );
        virtual action* check( const QString& type ){return 0;};

    private:
        QString filename_;
    };

    */

    QVariantMap from_json(const QByteArray& data);
    QByteArray to_json(const QVariantMap& map);
    
} // namespace remote


#endif
