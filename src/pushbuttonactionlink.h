#ifndef PUSHBUTTONACTIONLINK_H
#define PUSHBUTTONACTIONLINK_H

#include <QObject>

class QPushButton;
class QAction;

/*! \brief Link QAction to QPushButton

	\author Navrocky Vladislaw (navrocky@visualdata.ru)
*/
class push_button_action_link : public QObject
{
Q_OBJECT
public:
    push_button_action_link(QObject* parent, QPushButton *button, QAction* action);
private slots:
    void assign();
private:
    QPushButton* button_;
    QAction* action_;
};

#endif
