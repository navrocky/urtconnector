#ifndef APPOPTIONS_H
#define APPOPTIONS_H

#include <QString>

#include <serveroptions.h>

class AppOptions
{
public:
    AppOptions();
    ~AppOptions();

    void operator =( const AppOptions& src );

    /*! Use advanced command line advCmdLine() to launch UrT binary instead of binaryPath(). */
    bool useAdvCmdLine() const { return useAdvCmdLine_; }
    void setUseAdvCmdLine( bool value );

    /*! Advanced command line to launch UrT binary. Use followed substitutions:
        %host% %port% %user% %password% %rcon% and so on.*/
    QString advCmdLine() const { return advCmdLine_; }
    void setAdvCmdLine( const QString& value );

    /*! Path to UrT binary, used when not advanced command line. */
    QString binaryPath() const { return binaryPath_; }
    void setBinaryPath( const QString& value );

    /*! Servers options list. */
    ServerOptionsList& servers() {return servers_;}

    /*! Path to QStat binary */
    QString qstatPath() const {return qstatPath_;}
    void setQStatPath(const QString&);

private:
    bool useAdvCmdLine_;
    QString advCmdLine_;
    QString binaryPath_;
    ServerOptionsList servers_;
    QString qstatPath_;
};

typedef AppOptions* AppOptionsPtr;

#endif
