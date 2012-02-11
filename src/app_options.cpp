#include "app_options.h"

#include <QDate>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>

#include <settings/settings_generator_impl.h>
#include "config.h"


SETTINGS_GENERATE_CLASS_IMPL(app_settings, APP_SETTINGS)
SETTINGS_GENERATE_CLASS_IMPL(clip_settings, CLIP_SETTINGS)

bool is_christmas_mode()
{
    int month = QDate::currentDate().month();
    return ( month == 1 || month == 12 ) && app_settings().use_holiday_mode();
}

boost::shared_ptr<QTranslator> system_translator(const QString& code)
{
    boost::shared_ptr<QTranslator> ret( new QTranslator );
    ret->load( "qt_" + code, QLibraryInfo::location(QLibraryInfo::TranslationsPath) );
    return ret;
}

boost::shared_ptr< QTranslator > local_translator(const QString& code)
{
    boost::shared_ptr<QTranslator> ret( new QTranslator );

#if defined(Q_OS_UNIX)
    ret->load("urtconnector_" + code, "/usr/share/urtconnector/translations");
#elif defined(Q_OS_WIN)
    ret->load("urtconnector_" + code);
#elif defined(Q_OS_MAC)
    // FIXME i don't know how do this on mac
    ret->load(code);
#endif
    return ret;
}
