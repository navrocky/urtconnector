
#ifndef URT_RCON_SETTINGS_H
#define URT_RCON_SETTINGS_H

#include <QColor>
#include <QSettings>

#include <settings/settings.h>

class rcon_settings: public settings_uid_provider<rcon_settings>
{
public:

    enum Color{
        Background,
        Text,
        Command,
        Info,
        Error
    };
    
    bool custom_colors(){
        return part()->value(
            "custom_colors",
            //export from old config
            !( part()->value("adaptive_pallete", true).toBool() )
        ).toBool();
    }

    void set_custom_colors( bool b ){
        return part()->setValue( "custom_colors", b );
    }

    QColor color( Color c, bool reset = false ){

        QColor def;
        switch(c)
        {
            case Background:def = Qt::black; break;
            case Text:      def = Qt::gray;  break;
            case Command:   def = Qt::yellow;break;
            case Info:      def = Qt::cyan;  break;
            case Error:     def = Qt::red;   break;
        }

        if( reset ) part()->remove( QString("color_%1").arg(c) );
        
        return part()->value( QString("color_%1").arg(c), def ).value<QColor>();
    }

    void set_color( Color c, const QColor& color ){
         return part()->setValue( QString("color_%1").arg(c), color );
    }


};

#endif

