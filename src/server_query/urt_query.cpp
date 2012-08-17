#include "urt_query.h"

#include <QEvent>
#include <QHostInfo>
#include <QStringList>

#include <common/qt_syslog.h>

SYSLOG_MODULE(urt_query)

namespace
{

const quint32 c_q3_signature = 0xffffffff;

QByteArray read_data(QDataStream& ds, int size)
{
    QByteArray res(size, 0);
    int readed = ds.readRawData(res.data(), size);
    if (readed != size)
        res.resize(readed);
    return res;
}

bool check_signature(QDataStream &ds)
{
    quint32 signature;
    int readed = ds.readRawData(reinterpret_cast<char*>(&signature), sizeof(signature));
    if (readed != sizeof(signature))
        return false;
    if (signature != c_q3_signature)
        return false;
    return true;
}

bool check_string(QDataStream& ds, const QByteArray &str)
{
    QByteArray readed = read_data(ds, str.size());
    if (readed != str)
        return false;
    return true;
}

}

////////////////////////////////////////////////////////////////////////////////
// urt_query_options

urt_query_options::urt_query_options()
    : timeout(2000)
    , series_timeout(100)
    , retries(2)
{
}

urt_query_options::urt_query_options(int timeout_, int series_timeout_, int retries_)
    : timeout(timeout_)
    , series_timeout(series_timeout_)
    , retries(retries_)
{
}

////////////////////////////////////////////////////////////////////////////////
// urt_query

urt_query::urt_query(QObject *parent)
    : QObject(parent)
    , dispatcher_(0)
    , registered_(false)
    , retries_(0)
    , timer_id_(0)
    , ping_(0)
    , status_(s_not_started)
{
}

urt_query::~urt_query()
{
    if (dispatcher_ && registered_)
        dispatcher_->query_unreg(addr_, this);
}

void urt_query::set_addr(const server_id &addr)
{
    Q_ASSERT(status_ != s_executing);
    addr_ = addr;
}

void urt_query::set_opts(const urt_query_options& val)
{
    Q_ASSERT(status_ != s_executing);
    opts_ = val;
}

bool urt_query::event(QEvent * e)
{
    if (e->type() == QEvent::Timer)
    {
        QTimerEvent* te = static_cast<QTimerEvent*>(e);
        if (timer_id_ != 0 && te->timerId() == timer_id_)
        {
            killTimer(timer_id_);
            timer_id_ = 0;
            timeout();
            return true;
        }
    }
    return QObject::event(e);
}

void urt_query::finish()
{
    if (status_ != s_executing)
        return;
    if (timer_id_)
    {
        killTimer(timer_id_);
        timer_id_ = 0;
    }
    if (dispatcher_ && registered_)
        dispatcher_->query_unreg(addr_, this);
    registered_ = false;
    status_ = s_finished;
}

bool urt_query::retry()
{
    if (retries_ >= opts_.retries)
        return false;
    finish();
    retries_++;
    LOG_HARD << "Query timeout. Retry %1 from %2.", retries_, opts_.retries;
    start();
    return true;
}

void urt_query::throw_error(const QString &msg)
{
    finish();
    status_ = s_error;
    emit error(msg);
}

void urt_query::send(const QByteArray& data)
{
    Q_ASSERT(dispatcher_ && registered_);
    dispatcher_->send_query(addr_, data);
}

void urt_query::start_timeout(int tm)
{
    if (timer_id_)
        killTimer(timer_id_);
    timer_id_ = startTimer(tm);
    Q_ASSERT(timer_id_ != 0);
}

void urt_query::send_command(const QString &cmd)
{
    QByteArray data;
    data.append(reinterpret_cast<const char*>(&c_q3_signature), sizeof(c_q3_signature));
    data.append(cmd.toLatin1());
    send(data);
}

void urt_query::host_looked_up(const QHostInfo& host)
{
    if (host.error() != QHostInfo::NoError)
    {
        throw_error(tr("Host \"%1\" lookup failed: %2").arg(addr_.host_name()).arg(host.errorString()));
        return;
    }

    if (host.addresses().size() == 0)
    {
        throw_error(tr("Host \"%1\" lookup failed: no any ip").arg(addr_.host_name()));
        return;
    }

    QString ip = host.addresses().first().toString();
    addr_.set_ip(ip);
    addr_.set_host_name(QString());

    status_ = s_not_started;
    start();
}

bool urt_query::do_process_reply(const QByteArray &data)
{
    ping_ = time_.elapsed();
    bool res = process_reply(data);
    return res;
}

void urt_query::start()
{
    Q_ASSERT(status_ != s_executing);
    status_ = s_executing;

    Q_ASSERT(!addr_.is_empty());
    if (addr_.ip().isEmpty())
    {
        QHostInfo::lookupHost(addr_.host_name(),
                              this, SLOT(host_looked_up(QHostInfo)));
    } else
    {
        dispatcher_->query_reg(this);
        registered_ = true;
        time_.start();
        ping_ = 0;
        exec();
    }
}

////////////////////////////////////////////////////////////////////////////////
// urt_get_server_list

urt_query_dispatcher::urt_query_dispatcher(QObject *parent)
    : QObject(parent)
    , sock_(new QUdpSocket(this))
    , send_errors_(0)
{
    connect(sock_, SIGNAL(readyRead()), SLOT(read_pending_datagrams()));
    bool res = sock_->bind();
    if (!res)
        LOG_ERR << "Can't bind UDP socket";
}

void urt_query_dispatcher::exec_query(urt_query * q)
{
    q->dispatcher_ = this;
    q->start();
}

void urt_query_dispatcher::cancel_all_queries()
{

}

void urt_query_dispatcher::read_pending_datagrams()
{
    while (sock_->hasPendingDatagrams())
    {
        LOG_HARD << "Read pending datagrams";
        QByteArray datagram;
        datagram.resize(sock_->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        sock_->readDatagram(datagram.data(), datagram.size(), &sender, &senderPort);

        // process datagram
        server_id id(sender.toString(), QString(), senderPort);
        QList<urt_query*> queries = queries_.values(id);
        bool processed = false;
        foreach (urt_query* q, queries)
        {
            if (q->do_process_reply(datagram))
            {
                processed = true;
                break;
            }
        }
        if (!processed)
            LOG_WARN << "Received datagram was ignored because no waiting query";
    }
}

void urt_query_dispatcher::send_query(const server_id& id, const QByteArray& data)
{
    qint64 res = sock_->writeDatagram(data, QHostAddress(id.ip()), id.port());
    if (res != data.size())
    {
        LOG_WARN << "Datagram write res: %1", res;
        send_errors_++;
    }

//    usleep(10000);
//    Q_ASSERT(res == data.size());
}

void urt_query_dispatcher::query_reg(urt_query * q)
{
    Q_ASSERT(!q->addr().is_empty());
    queries_.insert(q->addr(), q);
}

void urt_query_dispatcher::query_unreg(const server_id & id, urt_query* q)
{
    int num = queries_.remove(id, q);
    Q_ASSERT(num == 1);
}


////////////////////////////////////////////////////////////////////////////////

urt_get_server_list::urt_get_server_list(QObject *parent)
    : urt_query(parent)
    , first_reply_received_(false)
{
}

void urt_get_server_list::exec()
{
    send_command("getservers 68 full empty");
//    send_command("getservers 70 full empty");

    res_.clear();
    start_timeout(opts().timeout);
    first_reply_received_ = false;
}

#pragma pack(push,1)
union ip_union
{
    struct
    {
        char _1;
        char _2;
        char _3;
        char _4;

    } ip;
    quint32 ip_as_int;
};

union port_union
{
    struct
    {
        char _1;
        char _2;

    } field;
    quint16 port_as_int;
};
#pragma pack(pop)

bool urt_get_server_list::process_reply(const QByteArray &data)
{
    QDataStream ds(data);

    if (!check_signature(ds))
        return false;

    if (!check_string(ds, "getserversResponse"))
        return false;

    int cnt = 0;
    while (!ds.atEnd())
    {
        if (!check_string(ds, "\\"))
        {
            Q_ASSERT(false);
            break;
        }

        ip_union ip;
        if (ds.readRawData(reinterpret_cast<char*>(&ip), sizeof(ip)) != sizeof(ip))
        {
            Q_ASSERT(false);
            break;
        }

        // "\EOT" protocol 70
        if (ip.ip_as_int == 0x544F45)
            break;

        ip_union ip2;
        ip2.ip._1 = ip.ip._4;
        ip2.ip._2 = ip.ip._3;
        ip2.ip._3 = ip.ip._2;
        ip2.ip._4 = ip.ip._1;

        Q_ASSERT(ip2.ip._1 != 0 && ip2.ip._4 != 0);

        port_union port;
        if (ds.readRawData(reinterpret_cast<char*>(&port), sizeof(port)) != sizeof(port))
            break;
        port_union port2;
        port2.field._1 = port.field._2;
        port2.field._2 = port.field._1;

        Q_ASSERT(port2.port_as_int != 0);

        server_id id(QHostAddress(ip2.ip_as_int).toString(), QString(), port2.port_as_int);
        res_.append(id);
        cnt++;
    }
    LOG_HARD << "Portion received %1", cnt;
    first_reply_received_ = true;
    start_timeout(opts().series_timeout);
    return true;
}

void urt_get_server_list::timeout()
{
    if (first_reply_received_)
    {
        finish();
        emit finished(res_);
    }
    else
    {
        if (!retry())
        {
            finish();
            throw_error(tr("Timeout while query master"));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

urt_get_server_info::urt_get_server_info(QObject *parent)
    : urt_query(parent)
{
}

void urt_get_server_info::exec()
{
    send_command("getinfo");
    start_timeout(opts().timeout);
}

bool urt_get_server_info::process_reply(const QByteArray &data)
{
    QDataStream ds(data);

    if (!check_signature(ds))
        return false;

    if (!check_string(ds, "infoResponse\n"))
        return false;

    QByteArray ba = read_data(ds, data.size());
    QStringList sl = QString(ba).split('\\', QString::SkipEmptyParts);

    info_.clear();
    for (int i = 0; i < sl.size(); i += 2)
    {
        QString key = sl[i].trimmed();
        QString value = sl[i+1].trimmed();
        info_[key] = value;
    }
    LOG_HARD << "Info received: %1", QString(ba);

    finish();
    emit finished();

    return true;
}

void urt_get_server_info::timeout()
{
    if (!retry())
        throw_error(tr("Query server info timeout"));
}

////////////////////////////////////////////////////////////////////////////////

urt_get_server_status::urt_get_server_status(QObject *parent)
    : urt_query(parent)
    , player_rx_("^(-?\\d+) (\\d+) \"(.+)\"$")
{
}

void urt_get_server_status::exec()
{
    send_command("getstatus");
    start_timeout(opts().timeout);
}

bool urt_get_server_status::process_reply(const QByteArray &data)
{
    QDataStream ds(data);
    if (!check_signature(ds))
        return false;
    if (!check_string(ds, "statusResponse\n"))
        return false;

    QByteArray ba = read_data(ds, data.size());
    QStringList sl = QString(ba).split('\n', QString::SkipEmptyParts);

    if (sl.size() > 0)
    {
        QStringList sl2 = sl[0].split("\\", QString::SkipEmptyParts);
        info_.clear();
        for (int i = 0; i < sl2.size(); i += 2)
        {
            QString key = sl2[i].trimmed();
            QString value = sl2[i+1].trimmed();
            info_[key] = value;
        }

        players_.clear();
        for (int i = 1; i < sl.size(); i++)
        {
            if (!player_rx_.exactMatch(sl[i]))
                LOG_WARN << "Invalid player info: %1", sl[i];

            bool ok;
            player_t rec;
            rec.score = player_rx_.cap(1).toInt(&ok);
            if (!ok)
                LOG_WARN << "Invalid player score: %1", sl[i];
            rec.ping = player_rx_.cap(2).toInt(&ok);
            if (!ok)
                LOG_WARN << "Invalid player ping: %1", sl[i];
            rec.name = player_rx_.cap(3).trimmed();
            players_.append(rec);
        }
    }

    LOG_HARD << "Status received: %1", QString(ba);

    finish();
    emit finished();

    return true;
}

void urt_get_server_status::timeout()
{
    if (!retry())
        throw_error(tr("Query server status timeout"));
}
