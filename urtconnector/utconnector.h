#ifndef UTCONNECTOR_H
#define UTCONNECTOR_H

#include <QWidget>

namespace Ui
{
    class utconnectorClass;
}

class utconnector : public QWidget
{
    Q_OBJECT

public:
    utconnector(QWidget *parent = 0);
    ~utconnector();

private:
    Ui::utconnectorClass *ui;

private slots:
    void launchUrbanTerror();
    void getUrTExe();
};

#endif // UTCONNECTOR_H
