
#include <iostream>

#include <boost/circular_buffer.hpp>

#include <QAbstractItemView>
#include <QLineEdit>
#include <QApplication>
#include <QEvent>
#include <QKeyEvent>
#include <QStringList>
#include <QDesktopWidget>

#include "rcon_completer.h"

struct rcon_completer::Pimpl{

    Pimpl():history(15), it_history( history.end() ), r_it_history( history.rend() ){}
    
    QString separator;
    QLineEdit* edit;
    QAbstractItemView* items;
    boost::circular_buffer<QString> history;
    boost::circular_buffer<QString>::const_iterator it_history;
    boost::circular_buffer<QString>::const_reverse_iterator r_it_history;
};

rcon_completer::rcon_completer(QLineEdit* edit, QAbstractItemModel *model, QObject *parent)
    : QCompleter(model, parent)
    , p_( new Pimpl )
{
    p_->edit = edit;
    p_->items = popup();
    
    p_->edit->setCompleter(this);
    p_->edit->installEventFilter(this);
}

void rcon_completer::setSeparator(const QString &separator)
{ p_->separator = separator; }

QString rcon_completer::separator() const
{ return p_->separator; }

QStringList rcon_completer::splitPath(const QString &path) const
{
    if( p_->separator.isNull() ) return QCompleter::splitPath(path);

    return path.split(p_->separator);
}

QString rcon_completer::pathFromIndex(const QModelIndex &index) const
{
    if( p_->separator.isNull() ) return QCompleter::pathFromIndex(index);

    // navigate up and accumulate data
    QStringList dataList;
    for (QModelIndex i = index; i.isValid(); i = i.parent()) {
        dataList.prepend(model()->data(i, completionRole()).toString());
    }

    return dataList.join(p_->separator);
}

bool rcon_completer::eventFilter(QObject* o, QEvent* e)
{
    if (o == p_->edit && edit_event(e) )
        return true;
    else if ( o == p_->items && items_event(e) )
        return true;
    else
        return QCompleter::eventFilter(o, e);
}

bool rcon_completer::edit_event(QEvent* e)
{
    if( e->type() == QEvent::KeyPress )
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        switch ( event->key() )
        {
            case Qt::Key_Return:
                p_->history.push_front( p_->edit->text() );
                p_->it_history = p_->history.begin();
                p_->r_it_history = p_->history.rbegin();
                return false;
                break;
            case Qt::Key_Up:
//                 if( p_->it_history != p_->history.end() )
//                 {
//                     p_->edit->setText( *p_->it_history );
//                     ++p_->it_history;
//                     if( p_->it_history == p_->history.end() )
//                         p_->it_history = p_->history.begin();
// 
//                     p_->r_it_history = static_cast<boost::circular_buffer<QString>::const_reverse_iterator>( p_->it_history );
//                 }
                return false;
                break;
            case Qt::Key_Down:
//                 if( p_->r_it_history != p_->history.rend() )
//                 {
//                     p_->edit->setText( *p_->r_it_history );
//                     ++p_->r_it_history;
//                     if( p_->r_it_history == p_->history.rend() )
//                         p_->r_it_history = p_->history.rbegin();
// 
//                     p_->it_history =  p_->r_it_history.base();
//                 }

                return false;
                break;
            default:
                break;
        }
    }

    return false;
}

bool rcon_completer::items_event(QEvent* e)
{
    if( e->type() == QEvent::KeyPress )
    {
        QKeyEvent* event = static_cast<QKeyEvent*>(e);
        switch ( event->key() )
        {
            case Qt::Key_Tab:
                QCoreApplication::postEvent(p_->items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier) );
                return true;
                break;
            case Qt::Key_Backtab:
                QCoreApplication::postEvent(p_->items, new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier) );
                return true;
                break;
            case Qt::Key_Return:
                emit activated( p_->items->currentIndex() );
                return true;
            default:
                break;
        }
    }
    else if( e->type() == QEvent::Show )
        fix_items_geometry();
    else if( e->type() == QEvent::Move || e->type() == QEvent::Resize && p_->items->isVisible() )
        fix_items_geometry();

    return false;
}

void rcon_completer::fix_items_geometry()
{
    QRect geom = p_->items->geometry();
    QRect avail = QApplication::desktop()->availableGeometry();

    if( geom.y() + geom.height() > avail.y() + avail.height() )
    {
        geom.moveBottom( geom.y() - p_->edit->height() );
        p_->items->setGeometry( geom );
    }
}



