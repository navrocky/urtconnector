#include <QMessageBox>
#include <iostream>

#include "main_form.h"
#include "ui_main_form.h"
#include "exchange/exchange.hpp"
#include "rand/rand_gen.hpp"
#include "blowfish/blowfish.hpp"

exchange* exchange::_instance = NULL;
Rand_Gen* Rand_Gen::_instance = NULL;

main_form::main_form(QWidget* parent)
: QWidget(parent)
, ui_(new Ui_main_form)
{
    ui_->setupUi(this);
    connect(ui_->button, SIGNAL(clicked()), SLOT(button_clicked()));
}

struct AD
{
    std::string response;
    std::string key;
    std::string sid;
    bool error;
} AuthData;

void ParseAuth(const char* cdata, int count, QString password)
{
    AuthData.error = false;
    std::string data;
    data.resize(count);
    for (int i=0; i<count; ++i)
    {
        data.push_back(cdata[i]);
    }

    size_t start = data.find("[BEGINRESPONSE]",0);
    size_t stop = data.find("[ENDRESPONSE]",start);
    if (start == std::string::npos || stop == std::string::npos)
    {
        AuthData.error = true;
        return;
    }
    AuthData.response = data.substr(start+15,stop-start-15);

    start = data.find("[BEGINKEY]",stop);
    stop = data.find("[ENDKEY]",start);
    if (start == std::string::npos || stop == std::string::npos)
    {
        AuthData.error = true;
        return;
    }
    AuthData.key = data.substr(start+10,stop-start-10);

    start = data.find("[BEGINSID]",stop);
    stop = data.find("[ENDSID]",start);
    if (start == std::string::npos || stop == std::string::npos)
    {
        AuthData.error = true;
        return;
    }
    AuthData.sid = data.substr(start+10,stop-start-10);

    try
    {
        Blowfish blow;
        blow.SetPassword(password.toStdString());
        blow.DecryptString(AuthData.key,AuthData.key);
        blow.SetPassword(AuthData.key);
        blow.DecryptString(AuthData.sid,AuthData.sid);
    }
    catch (...)
    {
        AuthData.error = true;
    }
}

void main_form::button_clicked()
{
    if (ui_->edit2->text().length()==0 || ui_->edit2->text().length()==0)
    {
        QMessageBox::critical(this, "Error!", "You need to enter login and password!", QMessageBox::Ok);
        return;
    }
    exchange *ex = exchange::GetInstance();
    Rand_Gen *rand = Rand_Gen::GetInstance();
    rand->SetSalt(ui_->edit2->text().toStdString());
    ex->NewRequest(QString::fromStdString(rand->Get(16)));
    ex->AddVar("action", "AUTH");
    ex->AddVar("login", ui_->edit1->text());
    ex->Request();
    QByteArray ba = ex->getBuf().readAll();
    ParseAuth(ba.constData(), ba.count(), ui_->edit2->text());
    if (AuthData.error == true || AuthData.key.length()!=64 || AuthData.sid.length()!=64)
    {
        QMessageBox::critical(this, "Error!", "Auth failed!", QMessageBox::Ok);
        return;
    }
    else
    {
        QMessageBox::information(this, "Auth", "Auth success!", QMessageBox::Ok);
        return;
    }
//    std::cout << AuthData.error << std::endl;
//    std::cout << AuthData.key << std::endl;
//    std::cout << AuthData.sid << std::endl;
/*    for (size_t i=0; i<AuthData.key.length(); ++i)
    {
        std::cout << AuthData.key[i];
    }
    std::cout << std::endl;*/
//    std::cout << AuthData.key << std::endl;
//    std::cout << ex->getBuf().readAll().constData() << std::endl;
//    ex->NewRequest("");
//    ex->Request();
//    std::cout << ex->getBuf().readAll().constData() << std::endl;
//    while (!ex->Free())
//    {
//    }
//    std::cout << result.readAll().constData() << std::endl;
//    std::cout << qh.errorString().toUtf8().constData() << std::endl;
//    QMessageBox::information(this, ui_->edit1->text(), ui_->edit2->text(), QMessageBox::Ok, QMessageBox::Ok);
//    ui_->edit2->setText(ui_->edit1->text());
}


