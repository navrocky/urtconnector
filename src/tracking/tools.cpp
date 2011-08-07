#include "tools.h"

#include <QRegExp>

namespace tracking
{

void replace_msg_tags(QString& msg, const data_map_t& data)
{
    static QRegExp rx("(%(\\w+))");
    forever
    {
        int i = rx.indexIn(msg);
        if (i < 0)
            break;
        QString key = rx.cap(2);
        QString value = "###";
        data_map_t::const_iterator it = data.find(key);
        if (it != data.end())
            value = it.value();
        msg.replace(i, rx.cap(1).length(), value);
    }
}

}
