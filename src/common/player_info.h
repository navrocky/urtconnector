#ifndef PLAYER_INFO_H
#define PLAYER_INFO_H

#include <QList>
#include <QString>
#include <QVariant>

#include "implicit_sharing.h"

class player_info
{
public:
    QString nick_name() const {return d->nick_name_;}
    void set_nick_name(const QString& val) {d->nick_name_ = val;}

    int score() const {return d->score_;}
    void set_score(int val) {d->score_ = val;}

    int ping() const {return d->ping_;}
    void set_ping(int val) {d->ping_ = val;}

private:
    struct impl
    {
        QString nick_name_;
        int score_;
        int ping_;
    };
    implicit_sharing<impl> d;
};

Q_DECLARE_METATYPE(player_info);

typedef QList<player_info> player_info_list;

#endif
