#include "play_sound_action.h"

#include <QApplication>
#include <QAction>
#include <QSound>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QToolButton>
#include <QFileInfo>
#include <phonon/MediaObject>

#include <common/qbuttonlineedit.h>

#include "../context.h"
#include "../tools.h"

namespace tracking
{

////////////////////////////////////////////////////////////////////////////////
// play_sound_action_class

play_sound_action_class::play_sound_action_class(const context_p& ctx)
: action_class(ctx, "play_sound", QObject::tr("Play sound"),
               QObject::tr("Play system or specified sound"),
               QIcon("icons:play-sound.png"))
{

}

action_p play_sound_action_class::create()
{
    return action_p(new play_sound_action(shared_from_this()));
}

////////////////////////////////////////////////////////////////////////////////
// play_sound_action

play_sound_action::play_sound_action(const action_class_p& c)
: action_t(c)
{
}

action_t::result_t play_sound_action::execute()
{
    if (sound_file_.isEmpty())
        QApplication::beep();
    else
    {
        using namespace Phonon;
        if (!player_)
        {
            player_ = createPlayer(MusicCategory, MediaSource(sound_file_));
            player_->setParent(this);
        }
        player_->seek(0);
        player_->play();
    }
    return r_continue;
}

QWidget* play_sound_action::create_options_widget(QWidget* parent)
{
    return new play_sound_option_widget(this, parent);
}

void play_sound_action::save(settings_t& s)
{
    s["sound_file"] = sound_file_;
}

void play_sound_action::load(const settings_t& s)
{
    settings_t::const_iterator it = s.find("sound_file");
    if (it != s.end())
        set_sound_file(it.value().toString());
}

void play_sound_action::set_sound_file(const QString& val)
{
    if (sound_file_ == val)
        return;
    sound_file_ = val;
    delete player_;
    emit changed();
}

////////////////////////////////////////////////////////////////////////////////
// play_sound_option_widget

play_sound_option_widget::play_sound_option_widget(play_sound_action* action, QWidget* parent)
: QWidget(parent)
, action_(action)
{
    select_file_action_ = new QAction(QIcon("icons:choose-file.png"), tr("Select sound file"), this);
    connect(select_file_action_, SIGNAL(triggered()), SLOT(select_file()));

    QHBoxLayout* l = new QHBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);

    QToolButton* btn = new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setIcon(QIcon("icons:media-playback-start.png"));
    btn->setToolTip(tr("Test selected sound"));
    l->addWidget(btn);
    connect(btn, SIGNAL(clicked()), SLOT(test_sound()));

    file_edit_ = new QButtonLineEdit(this);
    file_edit_->addActionButton(select_file_action_);
    file_edit_->setToolTip(tr("Sound file name."));
    connect(file_edit_, SIGNAL(textChanged(QString)), SLOT(text_changed()));
    l->addWidget(file_edit_);
    file_edit_->setText(action_->sound_file());
}

void play_sound_option_widget::test_sound()
{
    action_->execute();
}

void play_sound_option_widget::text_changed()
{
    action_->set_sound_file(file_edit_->text());
}

void play_sound_option_widget::select_file()
{
    QFileInfo fi(file_edit_->text());
    QString dir = fi.absolutePath();
    QString file = QFileDialog::getOpenFileName(this, tr("Select sound file"), dir,
                                 tr("Sounds (*.wav *.ogg *.mp3);;All files (*.*)"));
    if (!file.isEmpty())
        file_edit_->setText(file);
}

}

