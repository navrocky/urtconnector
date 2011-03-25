#ifndef FRIEND_RECORD_H
#define	FRIEND_RECORD_H

#include <QString>

#include <common/implicit_sharing.h>

class friend_record
{
public:
    // nick name is primary key too
    const QString& nick_name() const {return d->nick_name_;}
    void set_nick_name(const QString& val) {d->nick_name_ = val;}

    // regexp to find friend in the players name list
    // if regexp is empty then nick_name is used for the search
    const QString& expression() const {return d->expression_;}
    void set_expression(const QString& val) {d->expression_ = val;}
    
    const QString& comment() const {return d->comment_;}
    void set_comment(const QString& val) {d->comment_ = val;}

    bool is_empty() const {return d->nick_name_.isEmpty();}

    bool operator<(const friend_record& src) const;
    bool operator==(const friend_record& src) const;

private:
    struct impl
    {
        QString nick_name_;
/*        bool use_expression_;*/
        QString expression_;
//         bool use_regexp_;
        QString comment_;
    };
    implicit_sharing<impl> d;
};

#endif	/* FRIEND_RECORD_H */

