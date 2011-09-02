#ifndef TRACKING_PLAY_SOUND_ACTION_H
#define	TRACKING_PLAY_SOUND_ACTION_H

#include <QPointer>
#include <QWidget>

#include "../action.h"

class QButtonLineEdit;

namespace tracking
{

class play_sound_action_class : public action_class
{
public:
    play_sound_action_class(const context_p& ctx);
    action_p create();
};

class play_sound_action : public action_t
{
    Q_OBJECT
public:
    play_sound_action(const action_class_p& c);

    const QString& sound_file() const {return sound_file_;}
    void set_sound_file(const QString&);

    result_t execute();
    QWidget* create_options_widget(QWidget* parent);

    virtual void save(settings_t& s);
    virtual void load(const settings_t& s);

private:
    QString sound_file_;
};

class play_sound_option_widget : public QWidget
{
    Q_OBJECT
public:
    play_sound_option_widget(play_sound_action* action, QWidget* parent);

private slots:
    void text_changed();
    void select_file();
    void test_sound();

private:
    QButtonLineEdit* file_edit_;
    QAction* select_file_action_;
    QPointer<play_sound_action> action_;
};

}

#endif	/* play_sound_action_H */

