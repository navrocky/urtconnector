#include <QApplication>
#include <QAction>
#include <QAbstractItemView>
#include <QAbstractItemModel>
#include <QLineEdit>
#include <QEvent>
#include <QKeyEvent>
#include <QTimerEvent>

#ifdef Q_WS_X11
#include <QX11Info>
#endif

#include "qtreesearcher.h"

QTreeSearcher::QTreeSearcher(QAbstractItemView* view, QObject* parent)
: QObject(parent)
, m_edit(0)
, m_view(view)
, m_timer(0)
{
    m_findAction = new QAction(tr("Find"), this);
    connect(m_findAction, SIGNAL(triggered()), SLOT(showFindWidget()));

    m_view->installEventFilter(this);
}

bool QTreeSearcher::event(QEvent* e)
{
    if (e->type() == QEvent::Timer && static_cast<QTimerEvent*>(e)->timerId() == m_timer)
    {
        killTimer(m_timer);
        m_timer = 0;
        findFirst(QString());
        e->accept();
        return true;
    }
    return QObject::event(e);
}

bool QTreeSearcher::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_view && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);
        bool modified = (ke->modifiers() & (Qt::ControlModifier | Qt::AltModifier | Qt::MetaModifier));
        if (!ke->text().trimmed().isEmpty() && !modified)
        {
            showFindWidget();
            QKeyEvent* new_ke = new QKeyEvent(QEvent::KeyPress, ke->key(),
                                              ke->modifiers(), ke->text());
            QApplication::postEvent(m_edit, new_ke);
            return true;
        }
    }

    if (watched == m_edit && event->type() == QEvent::KeyPress)
    {
        QKeyEvent* ke = static_cast<QKeyEvent*>(event);

        if (ke->key() == Qt::Key_Escape)
        {
            hideFindWidget();
            return true;
        }
        if (ke->key() == Qt::Key_Enter || ke->key() == Qt::Key_Return)
        {
            hideFindWidget();
            findFirst(m_edit->text());
            return true;
        }
        if (ke->key() == Qt::Key_Down)
        {
            applyPattern(m_edit->text());
            findNext();
            return true;
        }
        if (ke->key() == Qt::Key_Up)
        {
            applyPattern(m_edit->text());
            findPrev();
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void QTreeSearcher::showFindWidget()
{
    if (!m_edit)
    {
        m_edit = new QLineEdit(m_view);
        m_edit->setWindowFlags(Qt::Popup);
        m_edit->installEventFilter(this);
        connect(m_edit, SIGNAL(textEdited(const QString&)), SLOT(queFindFirst(const QString&)));

#ifdef Q_WS_X11
        if (QX11Info::isCompositingManagerRunning())
        {
            m_edit->setAttribute(Qt::WA_TranslucentBackground, true);
        }
#endif
    }

    m_edit->adjustSize();
    QSize sz = m_edit->size();
    QPoint pt = m_view->mapToGlobal(m_view->contentsRect().bottomRight());
    pt.setX(pt.x() - sz.width());
    pt.setY(pt.y() - sz.height());
    m_edit->move(pt);
    m_edit->clear();
    m_edit->show();
    m_edit->setFocus();
}

void QTreeSearcher::hideFindWidget()
{
    bool f = m_edit->hasFocus();
    m_edit->hide();
    if (f)
        m_view->setFocus();
}

void QTreeSearcher::findFirst(const QString& str)
{
    if (!str.isEmpty())
        applyPattern(str);
    m_view->setCurrentIndex(QModelIndex());
    findNext();
}

QModelIndex QTreeSearcher::getNextIndex(const QModelIndex& idx)
{
    QAbstractItemModel* model = m_view->model();
    if (idx.isValid())
    {
        if (model->hasChildren(idx))
        {
            // if it has children, then go to the first child
            int cols = model->columnCount(idx);
            int rows = model->rowCount(idx);
            if (cols > 0 && rows > 0)
                return model->index(0, 0, idx);
        }

        // try move to the next sibling
        int cols = model->columnCount(idx.parent());
        int rows = model->rowCount(idx.parent());
        if (cols > 0 && rows > 0)
        {
            if (idx.column() < cols - 1)
                return model->index(idx.row(), idx.column() + 1, idx.parent());
            if (idx.row() < rows - 1)
                return model->index(idx.row() + 1, 0, idx.parent());
        }

        // move to the next parent sibling
        QModelIndex i = idx;
        while (i.parent().isValid())
        {
            i = i.parent();

            int cols = model->columnCount(i.parent());
            int rows = model->rowCount(i.parent());
            if (cols > 0 && rows > 0)
            {
                if (i.column() < cols - 1)
                    return model->index(i.row(), i.column() + 1, i.parent());
                if (i.row() < rows - 1)
                    return model->index(i.row() + 1, 0, i.parent());
            }
        }
    }

    // no one current index - take the first index
    int cols = model->columnCount();
    int rows = model->rowCount();
    if (cols > 0 && rows > 0)
        return model->index(0, 0);
    else
        return QModelIndex();
}

QModelIndex QTreeSearcher::getLastIndex(const QModelIndex& parent)
{
    QAbstractItemModel* model = m_view->model();
    QModelIndex res = parent;
    while (model->hasChildren(res))
    {
        int cols = model->columnCount(res);
        int rows = model->rowCount(res);
        res = model->index(rows - 1, cols - 1, res);
    }
    return res;
}

QModelIndex QTreeSearcher::getPrevIndex(const QModelIndex& idx)
{
    QAbstractItemModel* model = m_view->model();
    if (idx.isValid())
    {
        // moving up and then moving last
        QModelIndex res;
        if (idx.column() > 0)
            res = model->index(idx.row(), idx.column() - 1, idx.parent());
        else if (idx.row() > 0)
            res = model->index(idx.row() - 1, 
                               model->columnCount(idx.parent()) - 1, idx.parent());
        if (res.isValid())
            return getLastIndex(res);

        // if no more moving up then move to parent
        res = idx.parent();
        if (res.isValid())
            return res;
    }

    // no one current index - take the last index
    return getLastIndex(QModelIndex());
}

void QTreeSearcher::find(bool forward)
{
    if (!m_pattern.isValid())
        return;

    QAbstractItemModel* model = m_view->model();

    QModelIndex idx = m_view->currentIndex();
    idx = forward ? getNextIndex(idx) : getPrevIndex(idx);
    QModelIndex start = idx;
    while (idx.isValid() && idx != m_view->currentIndex())
    {
        QString s = model->data(idx, Qt::DisplayRole).toString();
        if (m_pattern.indexIn(s) >= 0)
            break;
        idx = forward ? getNextIndex(idx) : getPrevIndex(idx);
        if (idx == start)
            break;
    }

    m_view->setCurrentIndex(idx);
    m_view->selectionModel()->select(idx, QItemSelectionModel::ClearAndSelect);
}

void QTreeSearcher::findNext()
{
    find(true);
}

void QTreeSearcher::findPrev()
{
    find(false);
}

void QTreeSearcher::applyPattern(const QString& s)
{
    // here you can create an advanced pattern generation
    m_pattern = QRegExp(s);
}

void QTreeSearcher::queFindFirst(const QString& s)
{
    if (m_timer)
        killTimer(m_timer);
    m_timer = startTimer(QApplication::keyboardInputInterval());
    applyPattern(s);
}
