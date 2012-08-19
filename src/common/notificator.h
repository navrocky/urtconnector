#ifndef HE846EBF2A5D64D9EACB32CCD5276A705
#define HE846EBF2A5D64D9EACB32CCD5276A705

#include <QFrame>

class QLabel;
class QIcon;
class QProgressBar;
class QPropertyAnimation;
class QAction;
class QGraphicsOpacityEffect;

class Notificator : public QFrame
{
    Q_OBJECT
public:
    Notificator(QWidget* parent);

    void notify(const QIcon& icon, const QString& title, const QString& message);

    bool autoDestroy() const {return autoDestroy_;}
    void setAutoDestroy(bool v) {autoDestroy_ = v;}

    static void showMessage(const QIcon& icon, const QString& title, const QString& message);

    bool event(QEvent*);

private slots:
    void showFinished();
    void copyToClip();

private:
    enum state_t
    {
        s_none,
        s_show,
        s_hide
    };

    void correctPosition();
    void hideWidget();
    void showWidget();

    QLabel* iconLab_;
    QLabel* titleLab_;
    QLabel* messageLab_;
    QProgressBar* progress_;
    bool autoDestroy_;
    QPropertyAnimation* progressAnimation_;
    QAction* copyAction_;
    QAction* closeAction_;
    QGraphicsOpacityEffect* opacityEffect_;
};

#endif
