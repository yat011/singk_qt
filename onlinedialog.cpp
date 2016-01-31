#include "onlinedialog.h"
#include "ui_onlinedialog.h"
#include "mainwindow.h"




OnlineDialog::OnlineDialog(MainWindow *main,QWidget *parent):QDialog(parent),ui(new Ui::OnlineDialog())
{
    ui->setupUi(this);
    connect(ui->hostBtn,SIGNAL(pressed()),main,SLOT(hostClicked()));
     connect(ui->clientBtn,SIGNAL(pressed()),main,SLOT(clientClicked()));
}

OnlineDialog::~OnlineDialog()
{
    delete ui;
}

QString OnlineDialog::getPortValue()
{
    return ui->portEdit->text().trimmed();
}

QString OnlineDialog::getHostValue()
{
    return ui->hostEdit->text().trimmed();
}
