#include <QApplication>
#include "main_form.h"

int main(int argc, char** argv)
{
    QApplication app(argc, argv);
    

    main_form form;
    form.show();


    return app.exec();
}