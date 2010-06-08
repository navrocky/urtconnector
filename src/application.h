#ifndef APPLICATION_H
#define APPLICATION_H

// #if QT_VERSION >= QT_VERSION_CHECK(4, 5, 0)
// #define USE_SINGLE_APP
// #endif

#ifdef USE_SINGLE_APP
#include "qtsingleapplication/qtsingleapplication.h"
#else
#include <QApplication>
#endif

class application :
#ifdef USE_SINGLE_APP
public QtSingleApplication
#else
public QApplication
#endif
{
    Q_OBJECT
public:
    application(int &argc, char *argv[]);
    ~application();

    bool notify(QObject * receiver, QEvent * event);
    void show_error(const QString& msg);
};

#endif
