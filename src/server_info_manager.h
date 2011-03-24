
#ifndef URT_SRV_INFO_MANAGER_H
#define URT_SRV_INFO_MANAGER_H

#include <QTextBrowser>
#include <QTextObjectInterface>

class QToolButton;

class widget_object : public QObject, public QTextObjectInterface
{
     Q_OBJECT
     Q_INTERFACES(QTextObjectInterface)

public:

    enum { WidgetFormat = QTextFormat::UserObject + 1 };
    enum { WidgetPtr = QTextFormat::UserProperty + 1, ScrollBarPtr };

    QSizeF intrinsicSize(QTextDocument *doc, int posInDocument,
                        const QTextFormat &format);

    void drawObject(QPainter *painter, const QRectF &rect, QTextDocument *doc,
                     int posInDocument, const QTextFormat &format);
};

#include <iostream>

class server_info;
class player_info;

class server_info_manager: public QTextBrowser {
    Q_OBJECT
public:
    server_info_manager( QWidget* parent );

    virtual ~server_info_manager();

    void set_server_info( const server_info& si );

Q_SIGNALS:
    ///This signal emited when manager wants to add player to favorites 
    void add_to_friend( const QString& player ) const;
    
protected:
    virtual void paintEvent(QPaintEvent* e);
    
private Q_SLOTS:
    void add_friend() const;
    
private:
    QString create_html_template( const server_info& si ) const;

    void regenerate_widgets( const server_info& si );

    QToolButton* create_friend_button( const player_info& player, QObject* lifetime_dispatcher );

    QString make_status(  const server_info& si  ) const;
    
private:
    //When widgets are layouted above QTextBrowser we associate them with textblock where they are.
    //We need it to hide widgets when they move out from QAbstractScrollArea visible surface.
    //List used because of multimple widgets can be associated with one QTextBlock
    typedef std::list<QWidget*> Widgets;
    typedef std::map<QTextBlock, Widgets> WidgetsByBlock;
    WidgetsByBlock widgets;
};



#endif


