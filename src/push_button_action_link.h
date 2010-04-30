#ifndef PUSHBUTTONACTIONLINK_H
#define PUSHBUTTONACTIONLINK_H

#include <QObject>

class QPushButton;
class QAction;

/*! Link QAction to QPushButton */
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
