#ifndef HISTORY_WIDGET_H
#define HISTORY_WIDGET_H

#include <QWidget>
#include <QTreeWidget>
#include "pointers.h"

namespace Ui {
    class history_widget;
}

class history_widget : public QWidget {
    Q_OBJECT
public:
    history_widget( QWidget *parent, history_p list);
    ~history_widget();
    QTreeWidget* tree() const;
    void update_history();
    int num_rows() const;

protected:
    void changeEvent(QEvent *e);

private slots:
    void filter_clear();

private:
    Ui::history_widget *ui_;
    history_p history_;
    void addItem(history_item_p item);
};

#endif // HISTORY_WIDGET_H
