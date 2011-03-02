#include <QSettings>

#include "history.h"
#include "history_item.h"
#include "app_options.h"
#include "app_options_saver.h"

history::history(QObject* parent)
: QObject(parent)
, max_records_(0)
, max_days_(0)
, history_file_(get_app_options_settings("history"))
{
    load();
    options_refresh();
}

void history::add(const server_id& id, const QString& server_name,
                  const QString& player_name, const QString& password)
{
    add(history_item(id, server_name, password, player_name, QDateTime::currentDateTime()));
}

void history::add(const history_item& item)
{
    list_.push_back(item);
    shorten();
    save();
    emit changed();
}

void history::shorten()
{
    if( max_records_ > 0 && list_.size() > max_records_ )
    {
        history_list_t::iterator end = list_.begin();
        std::advance( end, list_.size() - max_records_ );
        list_.erase( list_.begin(), end );
    }

    // remove old
    if (max_days_ > 0)
    {
        // TODO
        
    }
}

void history::save()
{
    history_file_->beginWriteArray("history");

    history_list_t::const_iterator it = list_.begin();

    for( int index=0; it != list_.end(); ++it, ++index ){
        history_file_->setArrayIndex( index );
        history_file_->setValue("date_time", it->date_time());
        history_file_->setValue("server_name", it->server_name());
        history_file_->setValue("address", it->id().address());
        history_file_->setValue("password", it->password());
        history_file_->setValue("player_name", it->player_name());
    }
    
    history_file_->endArray();
}

void history::load()
{
    list_.clear();
    QDateTime date_time;
    QString server_name, address, password, player_name;
    int size = history_file_->beginReadArray("history");
    for (int i = 0; i < size; i++)
    {
        history_file_->setArrayIndex(i);
        date_time = history_file_->value("date_time").toDateTime();
        server_name = history_file_->value("server_name").toString();
        address = history_file_->value("address").toString();
        password = history_file_->value("password").toString();
        player_name = history_file_->value("player_name").toString();
        add_from_file(date_time, server_name, address, password, player_name);
    }
    history_file_->endArray();
}

void history::remove(const history_item& item)
{
    list_.remove(item);
    save();
    emit changed();
}

void history::clear()
{
    list_.clear();
    save();
    emit changed();
}

void history::options_refresh()
{
    app_settings s;
    max_records_ = s.number_in_history();
    max_days_ = s.days_in_history();
    shorten();
    emit changed();
}

void history::add_from_file(const QDateTime& date_time, const QString& server_name,
                            const QString& address, const QString& password,
                            const QString& player_name)
{
    history_item item(server_id(address), server_name, password, player_name, date_time);
    add(item);
}
