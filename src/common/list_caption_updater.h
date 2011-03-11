#ifndef LIST_CAPTION_UPDATER_H
#define LIST_CAPTION_UPDATER_H

#include <QString>

class QWidget;

class list_caption_updater
{
public:
    list_caption_updater(QWidget* widget, const QString& caption);
    
    void set_visible_count(int);
    void set_total_count(int);
    
private:
    void update_caption();
    
    QWidget* widget_;
    QString caption_;
    int visible_count_;
    int total_count_;
};

#endif // LIST_CAPTION_UPDATER_H
