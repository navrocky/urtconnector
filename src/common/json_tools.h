#ifndef URT_JSON_TOOLS_H
#define URT_JSON_TOOLS_H

#include "QVariantMap"

QVariantMap from_json(const QByteArray& data);
QByteArray to_json(const QVariantMap& map);



#endif // URT_JSON_TOOLS_H
