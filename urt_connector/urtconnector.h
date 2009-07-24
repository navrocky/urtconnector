#ifndef URTCONNECTOR_H
#define URTCONNECTOR_H

#include <QWidget>

namespace Ui
{
    class urtconnectorClass;
}

class urtconnector : public QWidget
{
    Q_OBJECT

public:
    urtconnector(QWidget *parent = 0);
    ~urtconnector();

private:
    Ui::urtconnectorClass *ui;

private slots:
    void launchUrbanTerror();
    void getUrTExe();
};

#endif // URTCONNECTOR_H
