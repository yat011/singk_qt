#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QNetworkProxyFactory>
#include <QWebSettings>
#include <QtWebKitWidgets/QWebView>
#include <QUrl>
#include <QFile>
#include <QDebug>
#include <QListView>

class CustomQItem : public  QStandardItem{
private :
    int id  ;
public :
    CustomQItem(int id, QString t):QStandardItem(t),id(id){
    }

};


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    video = new VideoController(ui->webView,0);
    connect(video->api,SIGNAL(loaded(double)),this,SLOT(onLoaded(double)));
    connect(video->api,SIGNAL(timeChanged(double)),this,SLOT(setTime(double)));
    connect(video,SIGNAL(videoAdded(int,QString)),this,SLOT(videoAdded(int,QString)));
    connect(video,SIGNAL(videoOnPlay(int,QString)),this, SLOT(videoOnPlay(int,QString)));
    model = new QStandardItemModel(this);


    ui->listView->setModel(model);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playBtn_clicked()
{
    video->play();
}
void  MainWindow::onLoaded(double Duration){
    ui->timeSlider->setMaximum(Duration*10);
    ui->timeSlider->setValue(0);
}
 void MainWindow::setTime(double time){
    ui->timeSlider->setValue(time*10);
 }

void MainWindow::on_pauseBtn_clicked()
{
    video->pause();
}


void MainWindow::on_timeSlider_sliderReleased()
{
    video->seekTo(ui->timeSlider->value()/10.0);
}

void MainWindow::on_timeSlider_sliderPressed()
{
    video->pause();
}

void MainWindow::on_addBtn_clicked()
{
    video->addVideo(ui->linkEdit->text().trimmed());
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
    QMap<int,QStandardItem*>::iterator itr =  itemMap.find(id);
    if (itr!=itemMap.end()){
        qDebug() << "move from list";
        CustomQItem * item = dynamic_cast<CustomQItem*>(itr.value());
        qDebug() << item;
        model->removeRow(item->row());
        itemMap.remove(id);
        delete item;
    }
    ui->currentVideoDisplay->setText(title);
}
