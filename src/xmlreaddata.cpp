#include <QtXml>
#include "xmlreaddata.h"
#include <iostream>

XmlReadData::XmlReadData()
{

}

bool XmlReadData::readFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadWrite | QFile::Text))
    {
        // Error!
        return false;
    }
    reader.setDevice(&file);

    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isStartElement())
        {
            if (reader.name() == "appdata") {
                readAppdata();
                return true;
            } else {
                reader.raiseError(QObject::tr("Error! Not a valid appdata.xml file: missing <appdata> element."));
            }
        } else {
            reader.readNext();
        }
    }
    if (reader.hasError()) {
        return false;
    }
    file.close();
    return true;
}

void XmlReadData::readAppdata()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }
        if (reader.isStartElement()) {
            if (reader.name() == "window") {
                readWindow();
            } else {
                if (reader.name() == "options") {
                    readOptions();
                } else {
                    if (reader.name() == "profiles") {
                        //readProfiles();
                    } else {
                        skipUnknownElement();
                    }
                }
            }
        }
    }
}

void XmlReadData::readWindow()
{
    reader.readNext();
    while (!reader.isEndElement() && reader.name() != "window") {
        if (reader.isStartElement()) {
            if (reader.name() == "width") {
                std::cout << reader.readElementText().toStdString() << std::endl;
            } else {
                if (reader.name() == "height") {
                    std::cout << reader.readElementText().toStdString() << std::endl;
                } else {
                    skipUnknownElement();
                }
            }
            reader.readNext();
        } else {
            reader.readNext();
        }
    }
}

void XmlReadData::readOptions()
{
    reader.readNext();
    while (!reader.isEndElement() && reader.name() != "options") {
        if (reader.isStartElement()) {
            if (reader.name() == "binaryPath") {
                std::cout << reader.readElementText().toStdString() << std::endl;
            } else {
                if (reader.name() == "advCmdLine") {
                    std::cout << reader.attributes().value("use").toString().toStdString() << std::endl;
                    std::cout << reader.readElementText().toStdString() << std::endl;
                } else {
                    if (reader.name() == "autoUpdateInterval") {
                        std::cout << reader.readElementText().toStdString() << std::endl;
                    } else {
                        skipUnknownElement();
                    }
                }
            }
            reader.readNext();
        } else {
            reader.readNext();
        }
    }
}

void XmlReadData::skipUnknownElement()
{
    reader.readNext();
    while (!reader.atEnd()) {
        if (reader.isEndElement()) {
            reader.readNext();
            break;
        }
        if (reader.isStartElement()) {
            skipUnknownElement();
        } else {
            reader.readNext();
        }
    }
}
