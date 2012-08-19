#ifndef URT_QUERY_H
#define	URT_QUERY_H

#include <QUdpSocket>
#include <QByteArray>
#include <QMultiMap>
#include <QTime>
#include <QRegExp>
#include <QQueue>

#include <common/server_id.h>

class QHostInfo;

class urt_query_dispatcher;

class urt_query_options
{
public:
    urt_query_options();
    urt_query_options(int timeout_, int series_timeout_, int retries_);
    int timeout;
    int series_timeout;
    int retries;
};

class urt_query : public QObject
{
    Q_OBJECT
public:
    enum status_t
    {
        s_not_started,
        s_executing,
        s_finished,
        s_error
    };

    urt_query(QObject* parent);
    ~urt_query();

    server_id addr() const {return addr_;}
    void set_addr(const server_id& addr);

    const urt_query_options& opts() const {return opts_;}
    void set_opts(const urt_query_options&);

    bool event(QEvent *);

    int retries() const {return retries_;}
    int ping() const {return ping_;}

    status_t status() const {return status_;}

signals:
    void error(const QString& msg);

protected:
    void start();
    void finish();
    bool retry();
    void throw_error(const QString& msg = QString());

    virtual void exec() = 0;
    virtual bool process_reply(const QByteArray& data) = 0;

    void send(const QByteArray&);

    void start_timeout(int);
    virtual void timeout() {}

    // useful tools
    void send_command(const QString &cmd);

private slots:
    void host_looked_up(const QHostInfo&);

private:
    bool do_process_reply(const QByteArray& data);

    friend class urt_query_dispatcher;
    server_id addr_;
    urt_query_options opts_;
    urt_query_dispatcher* dispatcher_;
    bool registered_;
    int retries_;
    int timer_id_;
    int ping_;
    QTime time_;
    status_t status_;
};

class urt_query_dispatcher : public QObject
{
    Q_OBJECT
public:
    urt_query_dispatcher(QObject* parent = 0);

    void exec_query(urt_query*);

    int send_errors() const {return send_errors_;}

    int resend_interval() const {return resend_interval_;}
    void set_resend_interval(int val) {resend_interval_ = val;}

    void set_max_resend(int val) {max_resend_ = val;}
    int total_resended() const {return total_resended_;}

    bool event(QEvent *);

private slots:
    void read_pending_datagrams();

private:
    friend class urt_query;
    void send_query(const server_id&, const QByteArray&, int resend = 0);
    void query_reg(urt_query*);
    void query_unreg(const server_id&, urt_query *q);
    void resend();

    typedef QMultiMap<server_id, urt_query*> queries_t;
    QUdpSocket* sock_;
    queries_t queries_;
    int send_errors_;

    struct queue_rec
    {
        int send_num;
        server_id id;
        QByteArray data;
    };
    QQueue<queue_rec> resend_que_;
    int send_later_timer_;
    int resend_interval_;
    int max_resend_;
    int total_resended_;
};

class urt_get_server_list : public urt_query
{
    Q_OBJECT
public:
    urt_get_server_list(QObject* parent = 0);

signals:
    void finished(const server_id_list&);

protected:
    void exec();
    bool process_reply(const QByteArray& data);
    void timeout();

private:
    server_id_list res_;
    bool first_reply_received_;
    bool first_query_finished_;
};

typedef QMap<QString, QString> urt_server_info_t;

class urt_get_server_info : public urt_query
{
    Q_OBJECT
public:
    urt_get_server_info(QObject* parent = 0);

    urt_server_info_t info() const {return info_;}

signals:
    void finished();

protected:
    void exec();
    bool process_reply(const QByteArray& data);
    void timeout();

private:
    urt_server_info_t info_;
};

class urt_get_server_status : public urt_query
{
    Q_OBJECT
public:

    struct player_t
    {
        QString name;
        int ping;
        int score;
    };
    typedef QList<player_t> players_t;

    urt_get_server_status(QObject* parent = 0);

    urt_server_info_t info() const {return info_;}
    players_t players() const {return players_;}

signals:
    void finished();

protected:
    void exec();
    bool process_reply(const QByteArray& data);
    void timeout();

private:
    urt_server_info_t info_;
    players_t players_;
    QRegExp player_rx_;
};


#endif
