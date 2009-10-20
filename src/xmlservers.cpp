#include "xmlfile.h"
#include "xmlservers.h"

XmlServers::XmlServers()
{
    fileName = "/home/endenis/Development/Qt/Xml/Data/servers.xml"; // <-
    rootElement = "servers";
}

XmlServers::XmlServers(QString otherFileName) : XmlFile(otherFileName)
{
    rootElement = "servers";
}

void XmlServers::readRootElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isStartElement())
        {
            if (reader.name() == "servers")
            {
               readServersElement();
            }
            else
            {
                // Обработка ошибки в XML-форамте
                // reader.name().toString()
                return;
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void XmlServers::readServersElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if (reader.isStartElement())
        {
            if (reader.name() == "folder")
            {
                readFolderElement();
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void XmlServers::readFolderElement()
{
    // Атрибут name можно получить так: reader.attributes().value("name").toString()
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            reader.readNext();
            break;
        }
        if (reader.isStartElement())
        {
            if (reader.name() == "folder")
            {
                readFolderElement();
            }
            else if (reader.name() == "server")
            {
                readServerElement();
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void XmlServers::read()
{
    if (!openReadFile())
    {
        // Ошибка открытия файла
        return;
    }
    readRootElement();
    closeFile();
}

void XmlServers::readServerElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            if (reader.name() == "server")
            {
                reader.readNext();
                break;
            }
            else
            {
                reader.readNext();
                continue;
            }
        }
        if (reader.isStartElement())
        {
            if (reader.name() == "host")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "ip")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "port")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "comment")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "player")
            {
                readPlayerElement();
            }
            else if (reader.name() == "passwords")
            {
                readPasswordsElement();
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void XmlServers::readPlayerElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            if (reader.name() == "player")
            {
                reader.readNext();
                break;
            }
            else
            {
                reader.readNext();
                continue;
            }
        }
        if (reader.isStartElement())
        {
            if (reader.name() == "name")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "profile")
            {
                // reader.readElementText()
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}

void XmlServers::readPasswordsElement()
{
    reader.readNext();
    while (!reader.atEnd())
    {
        if (reader.isEndElement())
        {
            if (reader.name() == "passwords")
            {
                reader.readNext();
                break;
            }
            else
            {
                reader.readNext();
                continue;
            }
        }
        if (reader.isStartElement())
        {
            if (reader.name() == "password")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "rcon")
            {
                // reader.readElementText()
            }
            else if (reader.name() == "refere")
            {
                // reader.readElementText()
            }
            else
            {
                skipUnknownElement();
            }
        }
        else
        {
            reader.readNext();
        }
    }
}
