#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

/*! Needed to catch exceptions.

    \author Navrocky Vladislaw (navrocky@visualdata.ru)
*/
class Application : public QApplication
{
Q_OBJECT
public:
    Application(int &argc, char *argv[]);
    ~Application();

    bool notify ( QObject * receiver, QEvent * event );

    void showError(const QString& msg);

};

#endif
