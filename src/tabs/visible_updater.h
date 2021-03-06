#ifndef VISIBLE_UPDATER_H
#define VISIBLE_UPDATER_H

#include <QObject>

class QWidget;
class QAccumulatingConnection;

// updates widget only if its in visible state, or pends update call until widget 
// became visible

class visible_updater : public QObject
{
    Q_OBJECT
public:
    explicit visible_updater(QWidget* w, const char* slot, QObject* parent = 0);
    
    virtual bool eventFilter(QObject* , QEvent* );
    
public slots:
    void update_contents();
    
signals:
    void update_needed();
    
private slots:
    void invisible_update();
    
private:
    bool update_pended_;
    QWidget* w_;
    QAccumulatingConnection* conn_;
};

#endif // VISIBLE_UPDATER_H
