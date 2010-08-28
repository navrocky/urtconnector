
 #include "rcon_completer.h"
 #include <QStringList>

 rcon_completer::rcon_completer(QObject *parent)
     : QCompleter(parent)
 {
 }

 rcon_completer::rcon_completer(QAbstractItemModel *model, QObject *parent)
     : QCompleter(model, parent)
 {
 }

 void rcon_completer::setSeparator(const QString &separator)
 {
     sep = separator;
 }

 QString rcon_completer::separator() const
 {
     return sep;
 }

 QStringList rcon_completer::splitPath(const QString &path) const
 {
     if (sep.isNull()) {
         return QCompleter::splitPath(path);
     }

     return path.split(sep);
 }

 QString rcon_completer::pathFromIndex(const QModelIndex &index) const
 {
     if (sep.isNull()) {
         return QCompleter::pathFromIndex(index);
     }

     // navigate up and accumulate data
     QStringList dataList;
     for (QModelIndex i = index; i.isValid(); i = i.parent()) {
         dataList.prepend(model()->data(i, completionRole()).toString());
     }

     return dataList.join(sep);
 }