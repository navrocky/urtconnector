#ifndef _APP_OPTIONS_SAVER_H
#define	_APP_OPTIONS_SAVER_H

#include <QString>

#include <boost/shared_ptr.hpp>

#include "appoptions.h"

typedef boost::shared_ptr<QSettings> qsettings_p;

// get tuned QSettings object
qsettings_p get_app_options_settings();

void save_app_options(const app_options& opts);
void load_app_options(app_options& opts);

void save_server_favs(const app_options& opts);
void load_server_favs(app_options& opts);

#endif	/* _APP_OPTIONS_SAVER_H */

