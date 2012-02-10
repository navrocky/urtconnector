#ifndef URT_CONFIG_WIZARD_H
#define URT_CONFIG_WIZARD_H

#include <QWidget>

class config_wizard : QWidget{
    Q_OBJECT
public:

    ~config_wizard(){}

    static bool check_qstat_binary();
    static bool configure_qstat_binary();

    static bool check_urt_binary();
    static bool configure_urt_binary();

};



#endif // URT_CONFIG_WIZARD_H
