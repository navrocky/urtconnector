#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class application : public QApplication
{
    Q_OBJECT
public:
    application(int &argc, char *argv[]);
    ~application();

    bool notify(QObject * receiver, QEvent * event);
    void show_error(const QString& msg);
};

#endif
