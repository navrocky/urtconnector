
 #ifndef URT_RCON_COMPLETER_H
 #define URT_RCON_COMPLETER_H

#include <tr1/tuple>

#include <stdexcept>

#include <boost/function.hpp>

 #include <QCompleter>

#include <QStandardItemModel>
#include <QProxyModel>

#include <iostream>

#include <cl/syslog/syslog.h>

typedef std::tr1::tuple<int, std::string> Expanding;

typedef std::map<std::string, int> Expandings;

struct recurse_locker{
    bool& recurse;
    recurse_locker( bool& r ) :recurse(r){
        if( recurse )
            throw std::runtime_error("Recurse collision detected!");
        
        recurse = true;
    }

    ~recurse_locker(){
        recurse = false;
    }
};

inline QStringList simple_list(){
    QStringList ret;
    ret << "1" <<"2" <<"3" <<"4"<<"5"<<"6"<<"7"<<"8"<<"9";
/*
    std::cerr<<"--------SIMPLE LIAST ----------";*/
    
    return ret;
}

class my_item_model: public QStandardItemModel{
    Q_OBJECT
public:

    mutable bool in_resurse;
    mutable int counter_;


    typedef std::map<QPersistentModelIndex, int>    Ids;
    mutable Ids parent_ids_;

    typedef boost::function< QStringList () > Expander;
    typedef std::map<std::string, Expander> ExpandersByKey;
    
    mutable ExpandersByKey  expanders_;

    QRegExp expander_exp;
    
    my_item_model(QObject* parent):QStandardItemModel(parent), in_resurse(false), counter_(0), expander_exp("%(.*)%")
    {
        expanders_["jerry"] = simple_list;
    }
    
    virtual ~my_item_model(){};

    //Если у элемента есть дочерние %элементы% то этот элемент "расширяем"
    bool is_expandable( const QModelIndex& parent ) const {
        if( is_proxy(parent) || in_resurse ) return false;

        recurse_locker locker( in_resurse );

        int count = QStandardItemModel::rowCount( parent );
        for( int row = 0; row < count; ++row )
        {
            if( data( parent.child( row, 0 ) ).toString().indexOf( expander_exp ) != -1 )
                return true;
        }

        return false;
    }

    //Список ключевых слов, которые будут расширяться
    QStringList keywords( const QModelIndex& parent ) const {
        if( !is_expandable(parent) )
            throw std::runtime_error("index is not expandable!");
        
        if( in_resurse ) return QStringList();

        recurse_locker locker( in_resurse );
        
        QStringList kwds;
        QString text;
        int count = QStandardItemModel::rowCount( parent );
        for( int row = 0; row < count; ++row )
        {
            text = data( parent.child( row, 0 ) ).toString();
            if( text.indexOf( expander_exp ) != -1 )
                kwds.push_back( text.mid(1, text.size() -2 ) );
        }

        return kwds;
    }

    QStringList expand_index( const QModelIndex& parent ) const {

        QStringList ret;

        foreach( const QString& str, keywords( parent ) ){
            ret << expanders_[ str.toStdString() ]();
        }

        return ret;
    }
    

    //Является ли этот индекс виртуальным?
    bool is_proxy( const QModelIndex& index ) const
    {
        if( index.isValid() )
            return parent_by_id( index.internalId() ).isValid();
        else
            return false;
    }

    //Получить родительский элемент для любого(даже виртуального) игдекса
    QModelIndex parent_index( const QModelIndex& ind ) const
    {
        if( is_proxy(ind) )
            return parent_by_id( ind.internalId() );
        else
            return ind.parent();
    }

    virtual int rowCount( const QModelIndex& parent = QModelIndex() ) const
    {
        if( is_proxy( parent ) )
            return 0;
        else if( is_expandable( parent ) )
            return expand_index(parent).size();
        else
            return QStandardItemModel::rowCount( parent );
    }

    virtual int columnCount( const QModelIndex& parent = QModelIndex() ) const
    {
        if( is_proxy(parent) )
            return 0;
        else if( is_expandable( parent ) )
            return 0;
        else
            return QStandardItemModel::columnCount( parent );
    }

    virtual QModelIndex parent( const QModelIndex& child ) const
    {
        if( is_proxy(child) )
            return parent_index(child);
        else
            return QStandardItemModel::parent(child);
    }

    int generate_id( const QModelIndex& parent ) const {
        QPersistentModelIndex item(parent);
        
        if( parent_ids_.find(item) == parent_ids_.end() )
            return parent_ids_[item] = counter_++;

       return parent_ids_[item];
    }

    QModelIndex parent_by_id( int id ) const {
        for( Ids::const_iterator it = parent_ids_.begin(); it != parent_ids_.end(); ++it )
        {
            if ( it->second == id )
                return it->first;
        }
        return QModelIndex();
    }

    virtual QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const
    {
        if ( is_proxy( parent ) )
            return QModelIndex();
        else if( is_expandable(parent) )
            return createIndex( row, column, generate_id(parent) );
        else
            return QStandardItemModel::index(row, column, parent);
    }
    
    virtual QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const
    {
        if( is_proxy(index) )
        {
            QModelIndex p_index = parent_index(index);

            if( ( role == Qt::DisplayRole ) || ( role == Qt::EditRole ) )
                return QVariant( expand_index(p_index)[index.row()] );
            else
                return data( p_index, role );
        }

        QVariant data = QStandardItemModel::data(index, role);

        return data;
    }

    

    virtual bool hasChildren(const QModelIndex& parent = QModelIndex()) const
    {
//         LOG_HARD << "enter hasChildren";
        bool b = QStandardItemModel::hasChildren( parent );
//         LOG_HARD << "leave hasChildren";
        return b;
    }

    virtual bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
    {
//         LOG_HARD << "enter dropMimeData";
        bool b = QStandardItemModel::dropMimeData(data, action, row, column, parent);
//         LOG_HARD << "leave dropMimeData";
        return b;
    }

    virtual bool canFetchMore(const QModelIndex& parent) const
    {
//         LOG_HARD << "enter canFetchMore";
        bool b = QStandardItemModel::canFetchMore(parent);
//         LOG_HARD << "leave canFetchMore";
        return b;

    }

    virtual void fetchMore(const QModelIndex& parent)
    {
//         LOG_HARD << "enter fetchMore";
        QStandardItemModel::fetchMore(parent);;
//         LOG_HARD << "leave fetchMore";
    }

    virtual Qt::ItemFlags flags(const QModelIndex& index) const
    {
//         LOG_HARD << "enter flags";
        Qt::ItemFlags f;
        if ( is_proxy(index) )
            f = QStandardItemModel::flags( parent_index(index) );
        else
            f = QStandardItemModel::flags( index );
//         LOG_HARD << "leave flags";
        return f;
    }

    virtual bool insertColumns(int column, int count, const QModelIndex& parent = QModelIndex())
    {
//         LOG_HARD << "enter insertColumns";
        bool b = QStandardItemModel::insertColumns(column, count, parent);
//         LOG_HARD << "leave insertColumns";
        return b;
    }

    virtual bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex())
    {
//         LOG_HARD << "enter insertRows";
        bool b = QStandardItemModel::insertRows(row, count, parent);
//         LOG_HARD << "leave insertRows";
        return b;
    }

    virtual QMap< int, QVariant > itemData(const QModelIndex& index) const
    {
//         LOG_HARD << "enter itemData";
        QMap< int, QVariant > v = QStandardItemModel::itemData(index);
//         LOG_HARD << "leave itemData";
        return v;
    }

    virtual bool removeColumns(int column, int count, const QModelIndex& parent = QModelIndex())
    {
//         LOG_HARD << "enter removeColumns";
        bool b = QStandardItemModel::removeColumns(column, count, parent);
//         LOG_HARD << "leave removeColumns";
        return b;
    }

    virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex())
    {
//         LOG_HARD << "enter removeRows";
        bool b;
        if( is_proxy(parent) )
            b = parent_ids_.erase( parent_by_id( parent.internalId() ) );
        else
            b = QStandardItemModel::removeRows(row, count, parent);
//         LOG_HARD << "leave removeRows";
        return b;
    }

    virtual bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)
    {
//         LOG_HARD << "enter setData";
        bool b = QStandardItemModel::setData(index, value, role);
//         LOG_HARD << "leave setData";
        return b;
    }

    virtual QSize span(const QModelIndex& index) const
    {
//         LOG_HARD << "enter span";
        QSize b = QStandardItemModel::span(index);
//         LOG_HARD << "leave span";
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