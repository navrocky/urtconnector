
 #ifndef URT_RCON_COMPLETER_H
 #define URT_RCON_COMPLETER_H

#include <tr1/tuple>

 #include <QCompleter>

#include <QStandardItemModel>
#include <QProxyModel>

#include <iostream>

typedef std::tr1::tuple<int, std::string> Expanding;

typedef std::map<std::string, int> Expandings;

class my_item_model: public QStandardItemModel{
    Q_OBJECT
public:

    mutable bool in_resurse;
    mutable int counter_;
    mutable std::map<int, QPersistentModelIndex> parents_map;

    

    
    my_item_model(QObject* parent):QStandardItemModel(parent), in_resurse(false), counter_(0), expanding_recurse(false)
    {}
    virtual ~my_item_model(){};

    mutable bool expanding_recurse;
    Expandings create_expanding(const QModelIndex& parent ) const
    {
        static const QRegExp r_exp = QRegExp("^%[a-zA-Z0-1]+%$");
        Expandings exp;
        if( expanding_recurse ) exp;

        expanding_recurse = true;
        
        if( parent.isValid() )
        {
            QString text;
            int count = QStandardItemModel::rowCount( parent );
            for( int row = 0; row < count; ++row )
            {
                text = data( parent.child( row, 0 ) ).toString();
                if( text.indexOf( r_exp ) != -1 )
                {
                    std::cerr<<"FINDED TEXT:"<<text.toStdString()<<std::endl;
                    exp[ text.mid(1, text.size() -2 ).toStdString() ] = row;
                }
            }
        }
        return exp;
    }
    
    QString expand_command( const QModelIndex& parent ) const
    {
        if( in_resurse )
             return QString();

        QString ret;
        in_resurse = true;
        std::cerr<<"e1"<<std::endl;
        if( parent.isValid() )
        {
            std::cerr<<"e2"<<std::endl;
            int count = QStandardItemModel::rowCount( parent );
            std::cerr<<"e3"<<std::endl;
            for( int row = 0; row < count; ++row )
            {
                std::cerr<<"e333"<<std::endl;
                if( data( parent.child( row, 0 ) ).toString() == "jerry" )
                {

                    create_expanding(parent);
                    
                    std::cerr<<"e4"<<std::endl;
                    ret = "jerry";
                    ;
                }
            }
        }
        std::cerr<<"e5"<<std::endl;
        in_resurse = false;
        return ret;
    }

    bool can_expand( const QModelIndex& parent ) const
    {
        if( in_resurse )
             return false;

        bool expandable = false;
        in_resurse = true;

        if( parent.isValid() )
        {
            int count = QStandardItemModel::rowCount( parent );
            for( int row = 0; row < count; ++row )
            {
                if( data( parent.child( row, 0 ) ).toString() == "jerry" )
                    expandable = true;
            }
        }
        in_resurse = false;
        return expandable;
    }

    bool is_proxy( const QModelIndex& index ) const
    {
        if( index.isValid() )
        {
            return parents_map.find( index.internalId() ) != parents_map.end();
        }
        else
            return false;
    }

    QModelIndex proxy_parent( const QModelIndex& ind ) const
    {
        if( is_proxy(ind) )
        { 
            QPersistentModelIndex i = parents_map[ ind.internalId() ];
            return index( i.row(), i.column(), i.parent() );
        }
        else
        {
            return ind.parent();
        }
        
    }
     
    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const
    {
        std::cerr<<"rowcount..."<<std::endl;
        if( is_proxy( parent ) )
        {
            return 0;
        }
        else if( expand_command( parent ).isEmpty() )
        {
            std::cerr<<"rowcount... ok"<<std::endl;
            return QStandardItemModel::rowCount( parent );
        }
        else
        {
            std::cerr<<"rowCount"<<std::endl;
            return 5;
        }
    }

    virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const
    {
        std::cerr<<"columnCount..."<<std::endl;
        QString expand = expand_command( parent );

        if( is_proxy(parent) )
        {
            return 0;
        }
        else if( expand.isEmpty() )
        {
            std::cerr<<"columnCount... ok"<<std::endl;
            return QStandardItemModel::columnCount( parent );
        }
        else
        {
            std::cerr<<"columnCount"<<std::endl;
            return 0;
        }
    }

    virtual QModelIndex parent( const QModelIndex& child ) const
    {
        std::cerr<<"parent...."<<std::endl;
        if( is_proxy(child) )
        {
            std::cerr<<"parent... ok 1"<<std::endl;
            return proxy_parent(child);
        }
        else
        {
            std::cerr<<"parent... ok 2"<<std::endl;
            return QStandardItemModel::parent(child);
        }
    }

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
    {
        std::cerr<<"index..."<<std::endl;
        QString expand = expand_command( parent );

        if ( is_proxy( parent ) )
        {
            std::cerr<<"procy_index"<<std::endl;
            return QModelIndex();
        }
        else if( expand.isEmpty() )
        {
            std::cerr<<"index...ok"<<std::endl;
            return QStandardItemModel::index(row, column, parent);
        }
        else
        {

            QModelIndex ind = createIndex( row, column, counter_ );
            parents_map[ counter_ ] = QPersistentModelIndex(parent);
            ++counter_;
            std::cerr<<"index created"<<std::endl;
            return ind;
        }
    }
    
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const
    {
        std::cerr<<"data..."<<std::endl;

        if( is_proxy(index) )
        {
            std::cerr<<"DATA for procy!!!"<<std::endl;
            if( role == Qt::DisplayRole )
                return QVariant( QString( "123123123123" ) );
            else
                return QVariant();
        }

        QVariant data = QStandardItemModel::data(index, role);

        if( data.canConvert<QString>() )
            std::cerr<<"DATA for REAL:"<<data.toString().toStdString()<<std::endl;
        
        std::cerr<<"data... ok"<<std::endl;
        return data;
    }

    

    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const
    {
        std::cerr<<"hasChildren..."<<std::endl;
        bool child = QStandardItemModel::hasChildren( parent );
        if( parent.isValid() )
        {
//             std::cerr<<"child for:"<<get_name(parent)<<" is:"<<child<<std::endl;;
        }
        std::cerr<<"hasChildren... ok"<<std::endl;
        return child;

    }

    virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
    {
        std::cerr<<"drop"<<std::endl;
        bool b = QStandardItemModel::dropMimeData(data, action, row, column, parent);
        std::cerr<<"drop ok"<<std::endl;
        return b;
    }

    virtual bool canFetchMore(const QModelIndex& parent) const
    {
        std::cerr<<"canFetchMore"<<std::endl;
        bool b = QStandardItemModel::canFetchMore(parent);
        std::cerr<<"canFetchMore ok"<<std::endl;
        return b;

    }

    

    virtual void fetchMore(const QModelIndex& parent)
    {
        std::cerr<<"fetch more..."<<std::endl;
        QStandardItemModel::fetchMore(parent);;
        std::cerr<<"fetch more ok"<<std::endl;
    }

    virtual Qt::ItemFlags flags(const QModelIndex& index) const
    {
        std::cerr<<"flags..."<<std::endl;
        
        Qt::ItemFlags f;
        if ( is_proxy(index) )
        {
            f = Qt::ItemIsEnabled;
            f |= Qt::ItemIsSelectable;
        }   
        else
            f = QStandardItemModel::flags(index);
        std::cerr<<"flags... OK"<<std::endl;
        return f;
    }

    virtual bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex())
    {
        std::cerr<<"insertColumns"<<std::endl;
        bool b = QStandardItemModel::insertColumns(column, count, parent);
        std::cerr<<"insertColumns ok"<<std::endl;
        return b;
    }

    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex())
    {
        std::cerr<<"insertRows"<<std::endl;
        bool b = QStandardItemModel::insertRows(row, count, parent);
        std::cerr<<"insertRows ok"<<std::endl;
        return b;
    }

    virtual QMap< int, QVariant > itemData(const QModelIndex& index) const
    {
        std::cerr<<"itemData"<<std::endl;
        QMap< int, QVariant > v = QStandardItemModel::itemData(index);
        std::cerr<<"itemData ok"<<std::endl;
        return v;
    }

    virtual bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex())
    {
        std::cerr<<"removeColumns"<<std::endl;
        bool b = QStandardItemModel::removeColumns(column, count, parent);
        std::cerr<<"removeColumns ok"<<std::endl;
        return b;
    }

    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex())
    {
        std::cerr<<"removeRows"<<std::endl;
        bool b = QStandardItemModel::removeRows(row, count, parent);
        std::cerr<<"removeRows ok"<<std::endl;
        return b;
    }

    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)
    {
        std::cerr<<"setData"<<std::endl;
        bool b = QStandardItemModel::setData(index, value, role);
        std::cerr<<"setData ok"<<std::endl;
        return b;
    }

    virtual QSize span(const QModelIndex& index) const
    {
        std::cerr<<"span"<<std::endl;
        QSize b = QStandardItemModel::span(index);
        std::cerr<<"span ok"<<std::endl;
        return b;
    }

};

 class rcon_completer : public QCompleter
 {
     Q_OBJECT
     Q_PROPERTY(QString separator READ separator WRITE setSeparator)

 public:
     rcon_completer(QObject *parent = 0);
     rcon_completer(QAbstractItemModel *model, QObject *parent = 0);

     QString separator() const;
 public slots:
     void setSeparator(const QString &separator);

 protected:
     QStringList splitPath(const QString &path) const;
     QString pathFromIndex(const QModelIndex &index) const;

 private:
     QString sep;
 };

 #endif // URT_RCON_COMPLETER_H