#include "history.h"
#include "history_item.h"
#include "app_options.h"
#include "app_options_saver.h"
#include <QSettings>

history::history()
    : max_( app_settings().number_in_history() )
    , history_file_(get_app_options_settings("history"))
{
    load();
}

void history::add(server_id id, QString server_name, QString player_name, QString password)
{
    QDateTime date_time = QDateTime::currentDateTime();
    history_item_p item_p(new history_item(id, server_name, password, player_name, date_time));
    add_history_item(item_p);
}

void history::shorten()
{
    if( list_.size() > max_ )
    {
        HistoryList::iterator end = list_.begin();
        std::advance( end, list_.size() - max_ );
        list_.erase( list_.begin(), end );
    }
}

void history::add_history_item(history_item_p item)
{
    list_.push_back(item);
    shorten();
    save();
}

void history::save()
{
    history_file_->beginWriteArray("history");

    HistoryList::const_iterator it = list_.begin();

    for( int index=0; it != list_.end(); ++it, ++index ){
        history_file_->setArrayIndex( index );
        history_file_->setValue("date_time", (*it)->date_time());
        history_file_->setValue("server_name", (*it)->server_name());
        history_file_->setValue("address", (*it)->address());
        history_file_->setValue("password", (*it)->password());
        history_file_->setValue("player_name", (*it)->player_name());
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

void history::change_max()
{
//     max_ = opts_->number_in_history;
    int oldLength = list_.size();
    shorten();
    if (oldLength != list_.size())
    {
        save();
    }
}

void history::add_from_file(QDateTime date_time, QString server_name, QString address, QString password, QString player_name)
{
    server_id id(address);
    history_item_p item_p(new history_item(id, server_name, password, player_name, date_time));
    add_history_item(item_p);
}
