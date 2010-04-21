#ifndef _QSTAT_UPDATER_H
#define	_QSTAT_UPDATER_H

#include <QObject>
#include <QProcess>
#include <QXmlStreamReader>

#include "qstat_options.h"
#include "serverlistcustom.h"

class qstat_updater : public QObject
{
    Q_OBJECT
public:
    qstat_updater(serv_list_custom* list, qstat_options* opts);

    void refresh_all();
    void refresh_selected(const server_id_list& list);
    void refresh_cancel();

signals:
    void refresh_stopped();

private slots:
    void error( QProcess::ProcessError error );
    void finished( int exitCode, QProcess::ExitStatus exit_status );
    void ready_read_output();

private:
    void process_xml();
    void prepare_info();

    QProcess proc_;
    QXmlStreamReader rd_;

    enum state_t
    {
        s_init, s_qstat, s_master_server, s_server, s_host_name, s_name,
        s_game_type, s_map, s_num_players, s_max_players, s_ping,
        s_retries, s_rules, s_rule, s_players, s_player, s_player_name,
        s_player_score, s_player_ping
    };
    state_t cur_state_;
    server_info cur_server_info_;
    player_info cur_player_info_;
    qstat_options* qstat_opts_;
    typedef std::pair<QString, QString> rule_info_t;
    rule_info_t cur_rule_;
    serv_list_custom* serv_list_;
};

#endif	/* _QSTAT_UPDATER_H */

