#ifndef PUSHBUTTONACTIONLINK_H
#define PUSHBUTTONACTIONLINK_H

#include <QObject>

class QPushButton;
class QAction;

/*! \brief Link QAction to QPushButton

	\author Navrocky Vladislaw (navrocky@visualdata.ru)
*/
class PushButtonActionLink : public QObject
{
Q_OBJECT
public:
    PushButtonActionLink(QObject* parent, QPushButton *button, QAction* action);
private slots:
    void assign();
private:
    QPushButton* button_;
    QAction* action_;
};

#endif
