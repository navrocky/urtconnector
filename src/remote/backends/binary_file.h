
#ifndef URT_BINARY_FILE_H
#define URT_BINARY_FILE_H

#include "remote/storage.h"

namespace remote {

    /*! simple test backend*/
    class binary_file_storage: public storage {
    public:

        binary_file_storage(const QString& filename): filename_(filename) {}

        virtual action* get( const QString& type );
        virtual action* put( const group& obj );
        virtual action* check(const QString& type){return 0;};

    private:
        QString filename_;
    };

} // namespace remote


#endif