#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkProxyFactory>
#include <QWebSettings>
#include <QtWebKitWidgets/QWebView>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QListView>
#include <QStyle>
class CustomQItem : public  QStandardItem{
private :

public :
    CustomQItem(int id, QString t):QStandardItem(t),id(id){
    }
    int id  ;
};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));


    /*
    connect(video->api,SIGNAL(loaded(double)),this,SLOT(onLoaded(double)));
    connect(video->api,SIGNAL(timeChanged(double)),this,SLOT(setTime(double)));
    */


    videoWidget = new QVideoWidget();
    videoWidget->show();
    ui->playerVerticalLayout->insertWidget(0,videoWidget);

    video = new VideoController(videoWidget,this);
    model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    connect(video,SIGNAL(videoAdded(int,QString)),this,SLOT(videoAdded(int,QString)));
    connect(video,SIGNAL(videoOnPlay(int,QString)),this, SLOT(videoOnPlay(int,QString)));
    connect(&video->player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));
    connect(&video->player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playBtn_clicked()
{
    video->play();
}


void MainWindow::on_pauseBtn_clicked()
{
    video->pause();
}


void MainWindow::on_timeSlider_sliderReleased()
{
    video->seekTo(ui->timeSlider->value());
}

void MainWindow::on_timeSlider_sliderPressed()
{
    video->pause();
}

void MainWindow::on_addBtn_clicked()
{
    if (ui->linkEdit->text().trimmed()=="c"){
       video->netController->connectToHost("localhost",1234);
        return;
    }else if (ui->linkEdit->text().trimmed()=="s"){
        video->netController->startServer(1234);
        return;
    }

    video->addVideo(ui->linkEdit->text().trimmed());
}

void MainWindow::showOnlineDialog()
{
    if (this->dialog == 0){
        dialog = new OnlineDialog(this);

    }
    dialog->show();
}
void MainWindow::videoAdded(int id,QString title){
    qDebug() << id << " " << title;
    CustomQItem* item = new CustomQItem(id,title);
    itemMap.insert(id,item);
    item->setEditable(false);
    model->appendRow(item);

    // if ...no video playing..

}

void MainWindow::videoOnPlay(int id, QString title)
{
    // some checking

    if (itemMap.contains(id)){
        qDebug() << "move from list";

        CustomQItem * item = dynamic_cast<CustomQItem*>(itemMap[id]);

        model->removeRow(item->row());

        itemMap.remove(id);

    }
    qDebug() << "on play";
    ui->currentVideoDisplay->setText(title);
}

void MainWindow::positionChanged(qint64 pos)
{
    //qDebug()<<pos;
    ui->timeSlider->setValue(pos);
}

void MainWindow::durationChanged(qint64 duration)
{
    qDebug()<<duration;
    ui->timeSlider->setRange(0,duration);
}

void MainWindow::on_actionOnline_triggered()
{
    showOnlineDialog();
}

void MainWindow::hostClicked()
{
    qDebug() << dialog->getPortValue();
    video->netController->startServer(dialog->getPortValue().toInt());
    dialog->hide();
}

void MainWindow::clientClicked()
{
    qDebug() << "try connect";
    video->netController->connectToHost(dialog->getHostValue(),dialog->getPortValue().toInt());
    dialog->hide();
}

void MainWindow::test()
{
    video->netController->startServer(1234);
    MainWindow * w2 = new MainWindow(this);
    w2->show();
    //w2->show();
   // w2->video->netController->connectToHost("localhost",1234);
   // MainWindow * w3 = new MainWindow();
    //w3->show();
   // w3->video->netController->connectToHost("localhost",1234);
}
