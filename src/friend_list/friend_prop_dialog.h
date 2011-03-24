#ifndef FRIEND_PROP_DIALOG_H
#define FRIEND_PROP_DIALOG_H

#include <memory>
#include <QDialog>
#include "friend_record.h"

class Ui_friend_prop_dialog;

class friend_prop_dialog : public QDialog
{
    Q_OBJECT
public:
    friend_prop_dialog(QWidget* parent);
    
    const friend_record& rec() const {return rec_;}
    void set_rec(const friend_record& rec);
        
    virtual void accept();

protected:
    
private:
    void update_contents();
    
    std::auto_ptr<Ui_friend_prop_dialog> ui_;
    friend_record rec_;
};

#endif // FRIEND_PROP_DIALOG_H
