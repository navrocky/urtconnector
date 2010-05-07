#ifndef _POINTERS_H
#define	_POINTERS_H

#include <boost/shared_ptr.hpp>

class app_options;
typedef boost::shared_ptr<app_options> app_options_p;

class server_list;
typedef boost::shared_ptr<server_list> server_list_p;

class QSettings;
typedef boost::shared_ptr<QSettings> qsettings_p;

class server_info;
///Shared pointer for server_info class. Forward declaration.
typedef boost::shared_ptr<server_info> server_info_p;

#endif	/* _POINTERS_H */

