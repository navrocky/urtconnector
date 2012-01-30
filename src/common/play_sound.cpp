#include "play_sound.h"

#include <QPointer>
#include <phonon/MediaObject>

namespace
{

class sound_player
{
public:
    ~sound_player()
    {
        delete player_;
    }

    void play(const QString& file)
    {
        using namespace Phonon;

        if (file != file_)
        {
            file_ = file;
            delete player_;
        }
        if (!player_)
            player_ = createPlayer(NotificationCategory, MediaSource(file_));
        player_->seek(0);
        player_->play();
    }

private:
    QPointer<Phonon::MediaObject> player_;
    QString file_;
};

sound_player player;

}

void play_sound(const QString& file_name)
{
    player.play(file_name);
}
