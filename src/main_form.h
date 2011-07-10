#ifndef MAIN_FORM_H
#define	MAIN_FORM_H

#include <memory>
#include <QWidget>
#include <QHttp>
#include <QBuffer>

class Ui_main_form;

class main_form : public QWidget
{
    Q_OBJECT
public:
    main_form(QWidget* parent = 0);

private slots:
    void button_clicked();

private:
    std::auto_ptr<Ui_main_form> ui_;
};

#endif	/* MAIN_FORM_H */

