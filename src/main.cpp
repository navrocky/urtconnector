#include <QApplication>
#include "urtconnector.h"

/* endenis@gmail.com */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    urtconnector w;
    w.show();
    return a.exec();
}
