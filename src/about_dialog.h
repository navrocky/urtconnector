#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <memory>

#include <QDialog>

namespace Ui
{
class AboutDialogClass;
}

class about_dialog : public QDialog
{
    Q_OBJECT
public:
    about_dialog(QWidget *parent = 0);
    ~about_dialog();

private:

    std::auto_ptr<Ui::AboutDialogClass> ui_;
};


struct snowflake{

    snowflake();
    
    snowflake( int x, int y, const QPixmap& pm );

    void tick();

    int x() const;
    int y() const;

    bool is_ok() const;

    const QPixmap& pixmap() const;
private:
    bool init_;
    
    float x_;
    float y_;

    float size;
    
    float hspeed_;
    float vspeed_;

    QPixmap pm_;
};


class blizzard: public QObject{
    Q_OBJECT
public:
    blizzard(QWidget* w);


    virtual bool eventFilter(QObject* o, QEvent* e);


private:
    QWidget* w_;

    QPixmap prototype;
    
    std::vector<snowflake> flakes_;
};





#endif // ABOUTDIALOG_H

