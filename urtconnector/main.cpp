#include <QApplication>
#include "utconnector.h"

/* endenis@gmail.com */

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    utconnector w;
    w.show();
    return a.exec();
}
