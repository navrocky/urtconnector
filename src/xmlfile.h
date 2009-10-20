#ifndef XMLFILE_H
#define XMLFILE_H

#include <QtXml>

class XmlFile
{
protected:
    QXmlStreamReader reader;
    QXmlStreamWriter writer;
    QString fileName;
    QFile *file;
    QString rootElement;

    bool openReadFile();
    bool openWriteFile();
    void closeFile();
    void skipUnknownElement();
    // bool itIsThisElement(QString);

public:
    XmlFile();
    XmlFile(QString);
};

#endif // XMLFILE_H
