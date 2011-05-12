/* 
 * File:   qtreesearcher.h
 * Author: vlad
 */

#ifndef QTREESEARCHER_H
#define	QTREESEARCHER_H

#include <QObject>
#include <QPointer>
#include <QModelIndex>
#include <QRegExp>

class QAbstractItemView;
class QAction;
class QLineEdit;

class QTreeSearcher : public QObject
{
    Q_OBJECT
public:
    QTreeSearcher(QAbstractItemView* view, QObject* parent = 0);

    virtual bool eventFilter(QObject* watched, QEvent* event);
    virtual bool event(QEvent*);

public slots:
    void showFindWidget();
    void hideFindWidget();
    void findFirst(const QString& str);
    void findNext();
    void findPrev();

private slots:
    void queFindFirst(const QString& str);

private:
    QModelIndex getNextIndex(const QModelIndex& idx);
    QModelIndex getPrevIndex(const QModelIndex& idx);
    QModelIndex getLastIndex(const QModelIndex& parent);
    void find(bool forward);
    void applyPattern(const QString& s);

    QAction* m_findAction;
    QAction* m_findNext;
    QAction* m_findPrev;
    QPointer<QLineEdit> m_edit;
    QRegExp m_pattern;
    QPointer<QAbstractItemView> m_view;
    int m_timer;
};

#endif	/* TREE_SEARCHER_H */

