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

private:
    std::auto_ptr<Ui::AboutDialogClass> ui_;
};

struct snowflake
{
    snowflake();
    snowflake(const QPointF& pos, const QPixmap & pm);

    void tick();

    const QPointF & pos() const
    {
        return pos_;
    }

    float size() const
    {
        return size_;
    }

    float rotate() const
    {
        return rotate_;
    }

    bool is_ok() const;

    const QPixmap & pixmap() const;

private:
    bool init_;
    QPointF pos_;
    float rotate_;
    float size_;
    float hspeed_;
    float vspeed_;
    float rotate_speed_;
    QPixmap pm_;
};

class blizzard : public QObject
{
    Q_OBJECT
public:
    blizzard(QWidget* w);
    virtual bool eventFilter(QObject* o, QEvent* e);

private slots:
    void animate();

private:
    QWidget* w_;
    QPixmap prototype_;
    std::vector<snowflake> flakes_;
};

#endif // ABOUTDIALOG_H

