
#ifndef URT_HISTORY_ITEM_H
#define URT_HISTORY_ITEM_H

#include <common/implicit_sharing.h>

class QDateTime;
class QString;

class server_id;


/**
 * @brief class to handle information about games
 **/
class history_item
{
public:
    history_item();

    history_item( const server_id&   id,
                  const QString&     server_name,
                  const QString&     password,
                  const QString&     player_name,
                  const QDateTime&   date_time );

    const server_id& id() const;
    const QString& server_name() const;
    const QString& password() const;
    const QString& player_name() const;
    const QDateTime& timestamp() const;

    bool operator==( const history_item& other ) const;
    bool operator<( const history_item& other ) const;
    
    bool is_empty() const;

protected:
    struct Pimpl;
    implicit_sharing<Pimpl> p_;
};

#endif // URT_HISTORY_ITEM_H
