#ifndef _APP_OPTIONS_SAVER_H
#define	_APP_OPTIONS_SAVER_H

#include "pointers.h"
#include "app_options.h"

/*! Returns tuned QSettings object.

    file_name - without extension
*/
qsettings_p get_app_options_settings(const QString& file_name);

void save_app_options(qsettings_p s, app_options_p opts);
void load_app_options(qsettings_p s, app_options_p opts);

void save_server_favs(qsettings_p s, app_options_p opts);
void load_server_favs(qsettings_p s, app_options_p opts);

#endif	/* _APP_OPTIONS_SAVER_H */

