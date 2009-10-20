#ifndef XMLSERVERS_H
#define XMLSERVERS_H

#include "xmlfile.h"

class XmlServers : public XmlFile
{
private:
    void readRootElement();
    void readServersElement();
    void readFolderElement();
    void readServerElement();
    void readPlayerElement();
    void readPasswordsElement();
public:
    XmlServers();
    XmlServers(QString);
    void read();
};

#endif // XMLSERVERS_H
