
#include "boost/bind.hpp"

#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QToolButton>
#include <QVBoxLayout>

#include <cl/except/error.h>

#include "tools.h"

#include "filter_list.h"
#include "filter_edit_widget.h"

#include "composite_filter.h"
#include "custom_filter.h"

using boost::bind;

typedef qt_signal_wrapper qsw;

custom_filter_editor::custom_filter_editor(custom_filter* filter, QWidget* parent)
    : QDialog( parent )
{
    QVBoxLayout* lay = new QVBoxLayout(this);

    QLineEdit* name = new QLineEdit( filter->name(), this);
    QObject::connect( name, SIGNAL( textChanged(const QString&) ),
        new qsw( name, bind( &custom_filter::set_name, filter, bind(&QLineEdit::text, name) ) ), SLOT( activate() ) );
    
    QDialogButtonBox* buttons = new QDialogButtonBox(this);
    buttons->setStandardButtons(QDialogButtonBox::Ok);
    connect(buttons, SIGNAL(accepted()), SLOT(accept()));
    connect(buttons, SIGNAL(rejected()), SLOT(reject()));

    lay->addWidget( name );
    lay->addWidget( new filter_edit_widget( filter->list(), this ) );
    lay->addWidget( buttons );       
}

custom_filter_class::custom_filter_class()
    : filter_class(get_id(), QObject::tr("Custom filter"),
        QObject::tr("Combine multiple filters into one entry") )
{}


QWidget* custom_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    custom_filter* filter = qobject_cast<custom_filter*>( f.get() );
    assert( filter );
    
    QWidget* frame = new QWidget(parent);
    QHBoxLayout* lay = new QHBoxLayout( frame );
    
    int left, top, right, bottom;
    lay->getContentsMargins ( &left, &top, &right, &bottom );
    lay->setContentsMargins(left,0,right,0);
    
    QToolButton* edit = new QToolButton( frame );
    edit->setIcon( QIcon::fromTheme("configure", QIcon("icons:configure.png") ) );
    edit->setAutoRaise(true);

    custom_filter_editor* editor = new custom_filter_editor( filter, edit );
    QObject::connect( edit, SIGNAL( clicked() ), editor, SLOT( show() ) );
    
    QLabel* name = new QLabel( filter->name(), frame );
    QObject::connect( filter, SIGNAL( changed_signal() ),
        new qsw( name, bind( &QLabel::setText, name, bind(&custom_filter::name, filter) ) ), SLOT( activate() ) );
    
    lay->addWidget( name );
    lay->addStretch();
    lay->addWidget( edit );
    return frame;
}

filter_p custom_filter_class::create_filter()
{ return filter_p( new custom_filter( shared_from_this() ) ); }

const char* custom_filter_class::get_id()
{ return "custom_filter"; }



custom_filter::custom_filter(filter_class_p fc)
    : filter(fc)
{}

bool custom_filter::filter_server(const server_info& si)
{ return list_->root_filter()->filter_server(si); }

QByteArray custom_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32)1; // version

    ds << name_ << list_->root_filter()->save();
    
    return res;
}

void custom_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version != 1)
        throw cl::except::error("Invalid filter version");

    QByteArray composite;
    
    ds >> name_ >> composite;

    list_->root_filter()->load(composite, factory);

    emit changed_signal();   
}

void custom_filter::set_name(const QString& name)
{ name_ = name; emit changed_signal(); }

const QString& custom_filter::name() const
{ return name_; }

void custom_filter::set_factory(filter_factory_p f)
{
    list_.reset( new filter_list( f ) );
    filter_p flt = list_->create_by_class_id(composite_filter_class::get_id());
    flt->set_name(flt->get_class()->caption());
    list_->set_root_filter(flt);
    
    connect(list_->root_filter().get(), SIGNAL( changed_signal() ), this, SIGNAL( changed_signal()) );
}

filter_factory_p custom_filter::factory()
{ return list_->factory(); }

filter_list_p custom_filter::list()
{ return list_; }

