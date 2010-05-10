#ifndef _APP_OPTIONS_SAVER_H
#define	_APP_OPTIONS_SAVER_H

#include "pointers.h"
#include "app_options.h"

// get tuned QSettings object
qsettings_p get_app_options_settings();

void save_server_favs(const app_options& opts);
void load_server_favs(app_options& opts);

#endif	/* _APP_OPTIONS_SAVER_H */

