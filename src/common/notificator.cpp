#include "notificator.h"

#include <QIcon>
#include <QLabel>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPalette>
#include <QTimerEvent>
#include <QApplication>
#include <QPointer>
#include <QVariant>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QGraphicsDropShadowEffect>
#include <QGraphicsBlurEffect>
#include <QMouseEvent>
#include <QAction>
#include <QClipboard>
#include <QStyle>

typedef QPointer<Notificator> notificator_p;

Q_DECLARE_METATYPE(notificator_p)

namespace
{
const int cShowTime = 2000;
const int cStepCount = 30;
const int cMargin = 10;
const char* cNotificatorPropertyName = "Notificator";
}

Notificator::Notificator(QWidget* parent)
: QFrame(parent)
, autoDestroy_(false)
{
    copyAction_ = new QAction(tr("Copy to the clipboard"), this);
    connect(copyAction_, SIGNAL(triggered()), SLOT(copyToClip()));
    closeAction_ = new QAction(style()->standardIcon(QStyle::SP_DialogCloseButton), tr("Close"), this);
    connect(closeAction_, SIGNAL(triggered()), SLOT(showFinished()));

    addAction(copyAction_);
    addAction(closeAction_);
    setContextMenuPolicy(Qt::ActionsContextMenu);

    setStyleSheet(
        "Notificator{ "
            "border-radius:5px;"
            "border: 2px solid gray;"
            "background-color: qlineargradient(spread:pad, x1:0, y1:0, x2:0.412, y2:1, stop:0 rgba(255, 255, 255, 255), stop:1 rgba(255, 239, 146, 255));"
        "}"
        "QProgressBar{"
            "border:none ;"
            "background:rgba(0, 0, 0, 0);"
        "}"
        "QProgressBar::chunk {"
            "background-color: rgb(200, 200, 0);"
            "border-radius:4px;"
        "}"
        "QLabel#title{"
            "font-weight: bold;"
        "}"
    );
    setAutoFillBackground(true);
    QBoxLayout* hl = new QHBoxLayout(this);
    progress_ = new QProgressBar(this);
    progress_->setMinimumSize(8, 8);
    progress_->setOrientation(Qt::Vertical);
    progress_->setTextVisible(false);
    hl->addWidget(progress_);
    iconLab_ = new QLabel(this);
    hl->addWidget(iconLab_);
    QBoxLayout* vl = new QVBoxLayout;
    vl->setContentsMargins(0, 0, 0, 0);
    hl->addLayout(vl);
    titleLab_ = new QLabel(this);
    titleLab_->setObjectName("title");
    vl->addWidget(titleLab_);
    messageLab_ = new QLabel(this);
    messageLab_->setObjectName("message");
    vl->addWidget(messageLab_);
    hide();

    progressAnimation_ = new QPropertyAnimation(progress_, "value", this);
    progressAnimation_->setDuration(cShowTime);
    progressAnimation_->setStartValue(cStepCount);
    progressAnimation_->setEndValue(0);
    connect(progressAnimation_, SIGNAL(finished()), SLOT(showFinished()));

//    opacityEffect_ = new QGraphicsOpacityEffect(this);
//    setGraphicsEffect(opacityEffect_);
//    opacityEffect_->setOpacity(0.5);

//    QGraphicsDropShadowEffect* eff = new QGraphicsDropShadowEffect(this);
//    eff->
//    setGraphicsEffect(eff);
//    eff->setBlurRadius(10);
//    eff->setColor(Qt::black);
//    eff->setXOffset(2);
//    eff->setYOffset(2);

    setAttribute(Qt::WA_Hover, true);
}

void Notificator::notify(const QIcon& icon, const QString& title, const QString& message)
{
    hide();
    iconLab_->setPixmap(icon.pixmap(32));
    titleLab_->setText(title);
    titleLab_->setVisible(!title.isEmpty());
    messageLab_->setText(message);
    progress_->setMinimum(0);
    progress_->setMaximum(cStepCount);
    progress_->setValue(cStepCount);
    progressAnimation_->stop();
    progressAnimation_->start();
    show();
    correctPosition();
}

void Notificator::correctPosition()
{
    QRect r = parentWidget()->contentsRect();
    QSize ns = minimumSizeHint();
    r.setTop(r.bottom() - ns.height() - cMargin);
    r.setLeft(cMargin);
    r.setSize(ns);
    setGeometry(r);
}

void Notificator::showFinished()
{
    progressAnimation_->stop();
    hide();
    if (autoDestroy_)
        deleteLater();
}

void Notificator::copyToClip()
{
    QClipboard *clipboard = QApplication::clipboard();
    clipboard->setText(messageLab_->text());
}

void Notificator::showMessage(const QIcon& icon, const QString& title, const QString& message)
{
    QWidget* w = qApp->activeWindow();
    if (!w)
        return;

    notificator_p p;
    const QVariant& v = w->property(cNotificatorPropertyName);
    if (v.isValid() && v.canConvert<notificator_p>())
        p = v.value<notificator_p>();
    if (!p)
    {
        p = new Notificator(w);
        w->setProperty(cNotificatorPropertyName, QVariant::fromValue(p));
    }
    p->notify(icon, title, message);
}

bool Notificator::event(QEvent* e)
{
    if (e->type() == QEvent::HoverEnter)
    {
        progressAnimation_->pause();
    }
    else if (e->type() == QEvent::HoverLeave)
    {
        progressAnimation_->resume();
    }
    else if (e->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent* me = static_cast<QMouseEvent*>(e);
        if (me->button() == Qt::LeftButton)
            showFinished();
    }

    return QFrame::event(e);
}

void Notificator::hideWidget()
{

}

void Notificator::showWidget()
{

}
