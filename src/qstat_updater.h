#ifndef _QSTAT_UPDATER_H
#define	_QSTAT_UPDATER_H

#include <QObject>
#include <QProcess>
#include <QPointer>
#include <QXmlStreamReader>

#include <common/server_info.h>
#include <common/player_info.h>
#include <geoip/geoip.h>
#include "pointers.h"

class qstat_updater : public QObject
{
    Q_OBJECT
public:
    qstat_updater( server_list_p list, const geoip& gi );
    ~qstat_updater();

    void refresh_all();
    void refresh_selected(const server_id_list& list);

    /* total count servers to refresh */
    int get_count() const {return count_;}

    /* refreshing progress */
    int get_progress() const {return progress_;}

    void clear();

    /*! Clear offline items after full refresh */
    bool clear_offline() const {return clear_offline_;}
    void set_clear_offline(bool);

public slots:
    void refresh_cancel();

signals:
    void refresh_stopped();

private slots:
    void error( QProcess::ProcessError error );
    void finished( int exitCode, QProcess::ExitStatus exit_status );
    void ready_read_output();
    void refresh_finish();

private:
    void process_xml();
    void prepare_info();
    void qprocess_needed();
    void refresh_stop(bool clear_offline);

    QPointer<QProcess> proc_;
    QXmlStreamReader rd_;

    enum state_t
    {
        s_init, s_qstat, s_master_server, s_server, s_host_name, s_name,
        s_game_type, s_map, s_num_players, s_max_players, s_ping,
        s_retries, s_rules, s_rule, s_players, s_player, s_player_name,
        s_player_score, s_player_ping
    };
    state_t cur_state_;
    server_info_p cur_server_info_;
    player_info cur_player_info_;
    typedef std::pair<QString, QString> rule_info_t;
    rule_info_t cur_rule_;
    server_list_p serv_list_;
    int count_;
    int progress_;
    bool canceled_;
    geoip gi_;

    QString qstat_output_;
    bool clear_offline_;
    server_info_list old_list_;
};

#endif	/* _QSTAT_UPDATER_H */

