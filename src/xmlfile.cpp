#include "xmlfile.h"

XmlFile::XmlFile()
{
}

XmlFile::XmlFile(QString otherFileName)
{
    fileName = otherFileName;
}

bool XmlFile::openReadFile()
{
    file = new QFile(fileName);
    if (!file->open(QFile::ReadOnly | QFile::Text))
    {
        // Обработка ошибки открытия файла для чтения.
        return false;
    }
    reader.setDevice(file);
    return true;
}

void XmlFile::closeFile()
{
    file->close();
    if (file->error())
    {
        // Обработка ошибки
        // file->errorString() - сообщение об ошибке.
    }
}

bool XmlFile::openWriteFile()
{
    file = new QFile(fileName);
    if (!file->open(QFile::WriteOnly | QFile::Text))
    {
        // Обработка ошибки открытия файла для записи.
        return false;
    }
    writer.setDevice(file);
    return true;
}

void XmlFile::skipUnknownElement()
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
            skipUnknownElement();
        }
        else
        {
            reader.readNext();
        }
    }
}

/*
bool XmlFile::itIsThisElement(QString element)
{
    if (reader.name() == element)
    {
        return true;
    }
    else
    {
        return false;
    }
}
*/
