
#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <QHBoxLayout>
#include <QSpinBox>
#include <QComboBox>
#include <QLabel>

#include <cl/except/error.h>

#include "tools.h"
#include "ping_filter.h"

using boost::bind;
using boost::ref;

const char* c_ping = "ping";

namespace
{

typedef boost::function<void() > F;

//functor to combine multiple calls

struct compose
{
    F f1_, f2_, f3_;

    compose(F f1 = F(), F f2 = F(), F f3 = F()) : f1_(f1), f2_(f2), f3_(f3)
    {
    }

    void operator()()
    {
        if (f1_) f1_();
        if (f2_) f2_();
        if (f3_) f3_();
    }
};

//invoke compose function fromm itemData

void invoke_data(QComboBox* combo)
{
    combo->itemData(combo->currentIndex()).value<compose > ()();
}
}

typedef qt_signal_wrapper qsw;


Q_DECLARE_METATYPE(compose);

ping_filter_quick_opt_widget::ping_filter_quick_opt_widget(filter_p f, QWidget* parent)
: QWidget(parent)
, filter_(*static_cast<ping_filter*> (f.get()))
{
    QHBoxLayout* lay = new QHBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);

    QSpinBox* spin1 = new QSpinBox(this);
    QSpinBox* spin2 = new QSpinBox(this);
    QLabel* label = new QLabel(this);
    QComboBox* combo = new QComboBox(this);

    spin1->setMaximum(500);
    spin2->setMaximum(500);

    //Creating combo items and fill itemData with compose function
    combo->addItem(tr("more"), qVariantFromValue(compose(
                                                         compose(
                                                                 bind(&QSpinBox::show, spin1),
                                                                 bind(&QSpinBox::hide, spin2),
                                                                 bind(&QLabel::setText, label, tr("<= Ping"))),
                                                         bind(&ping_filter::set_type, ref(filter_), ping_filter::more))));

    combo->addItem(tr("less"), qVariantFromValue(compose(
                                                         compose(
                                                                 bind(&QSpinBox::hide, spin1),
                                                                 bind(&QSpinBox::show, spin2),
                                                                 bind(&QLabel::setText, label, tr("Ping <="))),
                                                         bind(&ping_filter::set_type, ref(filter_), ping_filter::less))));

    combo->addItem(tr("between"), qVariantFromValue(compose(
                                                            compose(
                                                                    bind(&QSpinBox::show, spin1),
                                                                    bind(&QSpinBox::show, spin2),
                                                                    bind(&QLabel::setText, label, tr("<= Ping <="))),
                                                            bind(&ping_filter::set_type, ref(filter_), ping_filter::between))));


    //tmp vars
    boost::function<void() > set_min = bind(&ping_filter::set_min, &filter_, bind(&QSpinBox::value, spin1));
    boost::function<void() > set_max = bind(&ping_filter::set_max, &filter_, bind(&QSpinBox::value, spin2));

    //function that update all controls from filter data
    compose update_from_filter(
        bind(&QSpinBox::setValue, spin1, bind(&ping_filter::min, ref(filter_))),
        bind(&QSpinBox::setValue, spin2, bind(&ping_filter::max, ref(filter_))),
        bind(&QComboBox::setCurrentIndex, combo, bind(&ping_filter::type, ref(filter_)))
    );

    connect(spin1, SIGNAL(valueChanged(int)), new qsw(spin1, set_min), SLOT(activate()));
    connect(spin2, SIGNAL(valueChanged(int)), new qsw(spin2, set_max), SLOT(activate()));
    connect(combo, SIGNAL(currentIndexChanged(int)), new qsw(combo, bind(invoke_data, combo)), SLOT(activate()));

    connect(&filter_, SIGNAL(changed_signal()), new qsw(combo, update_from_filter), SLOT(activate()));

    lay->addWidget(combo);
    lay->addWidget(spin1);
    lay->addWidget(label);
    lay->addWidget(spin2);
    lay->addStretch();

    update_from_filter();
    invoke_data(combo);
}

ping_filter_class::ping_filter_class()
: filter_class(get_id(), QObject::tr("Ping filter"),
               QObject::tr("Hides all servers that don't match selected ping"))
{
}

QWidget* ping_filter_class::create_quick_opts_widget(filter_p f, QWidget* parent)
{
    return new ping_filter_quick_opt_widget(f, parent);
}

filter_p ping_filter_class::create_filter()
{
    return filter_p(new ping_filter(shared_from_this()));
}

const char* ping_filter_class::get_id()
{
    return "ping_filter";
}

bool ping_filter::filter_server(const server_info& si, filter_context& ctx)
{
    bool res;
    switch (type_)
    {
        case more: res = si.ping >= borders_.first;
        case less: res = si.ping <= borders_.second;
        case between: res = si.ping >= borders_.first && si.ping <= borders_.second;
    }
    if (res && ctx.data)
        ctx.data->insert(c_ping, QString("%1").arg(si.ping));
    return res;
}

ping_filter::ping_filter(filter_class_p fc)
: filter(fc)
, type_(more)
{
}

QByteArray ping_filter::save()
{
    QByteArray res;
    QDataStream ds(&res, QIODevice::WriteOnly);

    ds << (qint32) 1; // version

    ds << borders_ << (qint32) type_;

    return res;
}

void ping_filter::load(const QByteArray& ba, filter_factory_p factory)
{
    QDataStream ds(ba);

    qint32 version;
    ds >> version;
    if (version != 1)
        throw cl::except::error("Invalid filter version");

    qint32 t;

    ds >> borders_ >> t;

    type_ = Type(t);

    emit changed_signal();
}


