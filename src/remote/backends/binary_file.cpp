
#include <stdexcept>

#include <QFile>
#include <QDataStream>

#include "../remote.h"
#include "binary_file.h"

using namespace std;
using namespace remote;

object binary_file_storage::get(const QString& type)
{
    QFile f(filename_);
    if ( f.open(QFile::ReadOnly ) ) {
        QDataStream stream(&f);
        QVariantMap data;

        stream >> data;

        return object(data);
    } else {
        throw std::runtime_error("can't get object");
    }
}

void binary_file_storage::put(const remote::object& obj)
{
    QFile f(filename_);
    if ( f.open(QFile::WriteOnly | QFile::Truncate) ) {
        QDataStream stream(&f);
        stream << obj.save();
    } else {
        throw std::runtime_error("can't put object");
    }
}