#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H

#include <QDialog>

#include "appoptions.h"

namespace Ui
{
    class OptionsDialogClass;
}


class OptionsDialog : public QDialog
{
Q_OBJECT
public:
    OptionsDialog(QWidget *parent = 0);
    ~OptionsDialog();
                                  
    void setOpts(AppOptionsPtr value);
protected:
    void accept();


private slots:
    void chooseBinary();
    void insertFilePath();

private:
    void updateDialog();

    Ui::OptionsDialogClass *ui;
    AppOptionsPtr opts_;

};

#endif
