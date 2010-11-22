
#ifndef URT_RCON_COMPLETER_H
#define URT_RCON_COMPLETER_H

#include <memory>

#include <QCompleter>

class QLineEdit;

class rcon_completer : public QCompleter
{
    Q_OBJECT
    Q_PROPERTY(QString separator READ separator WRITE setSeparator)

public:
    rcon_completer(QLineEdit* edit, QAbstractItemModel *model, QObject *parent = 0);

    QString separator() const;
public slots:
    void setSeparator(const QString &separator);

protected:
    QStringList splitPath(const QString &path) const;
    QString pathFromIndex(const QModelIndex &index) const;

    virtual bool eventFilter(QObject* o, QEvent* e);

private:

    bool edit_event( QEvent* e );
    bool items_event( QEvent* e );
    void fix_items_geometry();

    
private:
    struct Pimpl;
    std::auto_ptr<Pimpl> p_;
};

#endif // URT_RCON_COMPLETER_H