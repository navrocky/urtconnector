#ifndef _APP_OPTIONS_SAVER_H
#define	_APP_OPTIONS_SAVER_H

#include "pointers.h"
#include "app_options.h"
#include "server_bookmark.h"

/*! Returns tuned QSettings object.

    file_name - without extension
*/
qsettings_p get_app_options_settings(const QString& file_name);

/*void save_app_options(qsettings_p s, app_options_p opts);
void load_app_options(qsettings_p s, app_options_p opts);*/

void save_server_bookmarks(qsettings_p s, server_bookmark_list* bml);
void load_server_bookmarks(qsettings_p s, server_bookmark_list* bml);

#endif	/* _APP_OPTIONS_SAVER_H */

