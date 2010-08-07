#include "history.h"
#include "history_item.h"
#include "app_options.h"
#include "app_options_saver.h"
#include <QSettings>

history::history(app_options_p opts) :
        opts_(opts),
        max_(opts->number_in_history) ,
        history_file_(get_app_options_settings("history"))
{
    load();
}

void history::add(QString address, QString player_name, QString password)
{
    server_id id(address);
    add(id, player_name, password);
}

void history::add(server_id id, QString player_name, QString password)
{
    QDateTime date_time = QDateTime::currentDateTime();
    QString server_name = "";
    history_item_p item_p(new history_item(id, server_name, password, player_name, date_time));
    add_history_item(item_p);
}

void history::shorten()
{
    while (list_.length() > max_)
    {
        list_.removeFirst();
    }
}

void history::add_history_item(history_item_p item)
{
    list_ << item;
    shorten();
    save();
}

void history::save()
{
    history_file_->beginWriteArray("history");
    for (int i = 0; i < list_.length(); i++)
    {
        const history_item_p& at_i = at(i);
        history_file_->setArrayIndex(i);
        history_file_->setValue("date_time", at_i->date_time());
        history_file_->setValue("server_name", at_i->server_name());
        history_file_->setValue("address", at_i->address());
        history_file_->setValue("password", at_i->password());
        history_file_->setValue("player_name", at_i->player_name());
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
    max_ = opts_->number_in_history;
    int oldLength = length();
    shorten();
    if (oldLength != length())
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
