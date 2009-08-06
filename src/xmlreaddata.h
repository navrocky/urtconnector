#ifndef XMLREADDATA_H
#define XMLREADDATA_H
#include <QXmlStreamReader>
class XmlReadData
{
private:
    QXmlStreamReader reader;
    void skipUnknownElement();
    void readWindow();
    void readOptions();
//    void readProfiles();
public:
    XmlReadData();
    bool readFile(const QString &fileName);
    void readAppdata();
};

#endif // XMLREADDATA_H
