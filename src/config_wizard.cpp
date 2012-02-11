
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfo>
#include <QProcess>

#include "app_options.h"
#include "common/qstat_options.h"
#include "config_wizard.h"


bool config_wizard::check_qstat_binary() {
    qstat_options qs;

    if (!QFileInfo(qs.qstat_path()).exists() || !QFileInfo(qs.qstat_path()).isExecutable()) {
        return false;
    }

    QProcess proc;
    proc.start (qs.qstat_path());
    if (!proc.waitForFinished(3000)) {
        return false;
    }

    return proc.readAllStandardOutput().contains("Where host is an IP address or host name");
}

bool config_wizard::configure_qstat_binary() {

    qstat_options qs;

    while (!check_qstat_binary()) {
        QMessageBox::information(0, tr("qstat executable missing"), tr("No qstat(quakestat) executable found, please select executable manually"));

        const QString binary = QFileDialog::getOpenFileName(0, tr("Please select qstat(quakestat) executeable"));
        if (binary.isNull()) return false;

        qs.qstat_path_set(binary);
    }

    return true;
}

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
