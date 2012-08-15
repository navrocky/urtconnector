
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>

#include <common/qstat_options.h>
#include "app_options.h"
#include "config_wizard.h"


bool config_wizard::check_urt_binary() {
    app_settings as;
    return QFileInfo(as.binary_path()).exists() && QFileInfo(as.binary_path()).isExecutable();
}

bool config_wizard::configure_urt_binary() {

    app_settings as;

    while (!check_urt_binary()) {
        QMessageBox::information(0, tr("UrbanTerror executable missing"), tr("No UrbanTerror executable found, please select executable manually"));

        const QString binary = QFileDialog::getOpenFileName(0, tr("Please select UrbanTerror executeable"));
        if (binary.isNull()) return false;

        as.binary_path_set(binary);
    }

    return true;
}
