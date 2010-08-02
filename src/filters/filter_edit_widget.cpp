#include <QTreeWidget>

#include "filter_edit_widget.h"

filter_edit_widget::filter_edit_widget(filter_list_p filters, QWidget* parent)
: QWidget(parent)
, filters_(filters)
{
    
}
