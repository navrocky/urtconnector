
#ifndef URT_GDOCS_WIDGET_H
#define URT_GDOCS_WIDGET_H

#include <QDialog>

#include "manager.h"

#include "ui_gdocs_widget.h"

class gdocs_widget: public QDialog {
    Q_OBJECT
public:
    gdocs_widget(boost::shared_ptr<QSettings> s, QWidget* parent)
        : QDialog(parent)
        , settings_(s)
    {
        Ui_gdocs_widget ui;
        ui.setupUi(this);
    }
    
    boost::shared_ptr<QSettings> settings() {
        return settings_;
    }
  
    boost::shared_ptr<QSettings> settings_;
};


#endif

