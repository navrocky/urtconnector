#ifndef _POINTERS_H
#define	_POINTERS_H

#include <boost/shared_ptr.hpp>

class app_settings;
typedef boost::shared_ptr<app_settings> app_options_p;

class server_list;
typedef boost::shared_ptr<server_list> server_list_p;

class QSettings;
typedef boost::shared_ptr<QSettings> qsettings_p;

class server_info;
typedef boost::shared_ptr<server_info> server_info_p;

class history;
typedef boost::shared_ptr<history> history_p;

#endif	/* _POINTERS_H */
