//
// C++ Interface: preferences_widget
//
// Description: 
//
//
// Author: Samoilenko Yuri <kinnalru@gmail.com>, (C) 2010
//
// Copyright: See COPYING file that comes with this distribution
//
//


#ifndef CL_PREFERENCES_WIDGET_H
#define CL_PREFERENCES_WIDGET_H

#include <boost/shared_ptr.hpp>

#include <QWidget>


/*! \brief Базовый класс для виджетов используемых в конфигурационных диалогах
*/
class preferences_widget: public QWidget {
    Q_OBJECT
public:

    static const QString object_name;

    preferences_widget( QWidget* parent, const QString& name, Qt::WindowFlags f = 0);
    ~preferences_widget();

    ///Установить заголовок конфигурационного диалога
    void set_header( const QString& header );
    const QString& header() const;

    ///Установить текст конфигурационного диалога
    void set_name( const QString& header );
    const QString& name() const;

    ///Установить иконку конфигурационного диалога
    void set_icon( const QIcon& icon);
    const QIcon& icon() const;

Q_SIGNALS:
    ///сигнал о том, что есть изменения
    void changed();

    void enabled( bool b);
public Q_SLOTS:

    ///Этот слот вызывается перед показом виджета и загрузка всех настроек должна поисходить здесь
    virtual void update_preferences();
    
    ///Этот слот вызывается когда изменения приняты
    virtual void accept();

    ///Этот слот вызывается когда изменения отклонены
    virtual void reject();

    ///Этот слот вызывается чтоб узнановить значения по умолчанию
    virtual void reset_defaults();

    void setEnabled(bool b);

private:
    struct Pimpl;
    boost::shared_ptr<Pimpl> p_;
};


#endif


