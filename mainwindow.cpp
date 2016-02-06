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
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QTreeWidgetItem>
#include <QQuickView>
#include <QQmlComponent>
#include <QQmlProperty>
#include <QQmlEngine>
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
    ui->playBtn->setText("");
    ui->pauseBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    ui->pauseBtn->setText("");
    ui->pauseBtn->hide();
    ui->fullScreenBtn->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
    ui->fullScreenBtn->setText("");
    ui->fullScreenBtn->hide();

    /*
    connect(video->api,SIGNAL(loaded(double)),this,SLOT(onLoaded(double)));
    connect(video->api,SIGNAL(timeChanged(double)),this,SLOT(setTime(double)));
    */


    videoWidget = new QVideoWidget();
    //videoWidget->setParent(ui->playArea);
   // videoWidget->show();
   // videoWidget->setS
    //ui->info_label->raise();
    QGraphicsOpacityEffect * eff = new QGraphicsOpacityEffect(this);
   // ui->info_label->setGraphicsEffect(eff);
   // ui->info_label->setText("");
    QPropertyAnimation *a = new QPropertyAnimation(eff,"opacity");
    a->setDuration(0);
    a->setStartValue(1);
    a->setEndValue(0.7);
    a->setEasingCurve(QEasingCurve::InBack);
    a->start(QPropertyAnimation::DeleteWhenStopped);

    qmlVideo = new QQuickView();
    qmlVideo->engine()->addImportPath("C:\\Qt\\5.5\\msvc2013_64\\qml");
    container = QWidget::createWindowContainer(qmlVideo);
    container->setStyleSheet("background-color: rgb(7, 7, 7);");
    qmlVideo->setMinimumSize(QSize(500,500));
     qmlVideo->setSource(QUrl("qrc:/video.qml"));
     qmlVideo->setColor(QColor(0,0,0,255));
     QObject * obj = (QObject* )qmlVideo->rootObject();
     QVariant m= "hellofhefa";
     QVariant d= 5000;
    QMetaObject::invokeMethod(obj,"showFlashMessage",Q_ARG(QVariant,m),Q_ARG(QVariant,d));
    ui->playArea->layout()->addWidget(container);

    video = new VideoController(videoWidget,this);
    model = new QStandardItemModel(this);
    ui->listView->setModel(model);

    ui->tableWidget->setColumnCount(3);
    QTableWidgetItem * h1 = new QTableWidgetItem();
    h1->setText("Id");

    QTableWidgetItem * h2 = new QTableWidgetItem();
    h2->setText("Ping");
    QTableWidgetItem * h3 = new QTableWidgetItem();
    h3->setText("State");
    ui->tableWidget->setHorizontalHeaderItem(0,h1);
    ui->tableWidget->setHorizontalHeaderItem(1,h2);
    ui->tableWidget->setHorizontalHeaderItem(2,h3);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->setColumnWidth(0,40);
     ui->tableWidget->setColumnWidth(1,70);
      ui->tableWidget->setColumnWidth(2,130);


    connect(video,SIGNAL(videoAdded(int,QString)),this,SLOT(videoAdded(int,QString)));
    connect(video,SIGNAL(videoOnPlay(int,QString)),this, SLOT(videoOnPlay(int,QString)));
    connect(&video->player,SIGNAL(durationChanged(qint64)),this,SLOT(durationChanged(qint64)));
    connect(&video->player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));
    connect (video,SIGNAL(consoleRead(QString)),this,SLOT(showConsoleMessage(QString)));
    connect(video->netController,SIGNAL(onlineSig(bool)),this,SLOT(online(bool)));
    connect(video->netController,SIGNAL(offline()),this,SLOT(offline()));
    connect (video->netController,SIGNAL(consoleMessage(QString)),this,SLOT(showConsoleMessage(QString)));
    connect(video->netController,SIGNAL(clientInitComplete()),this,SLOT(clientInitComplete()));
    connect(video,SIGNAL(resetPlayList()),this,SLOT(resetList()));
    connect(video->netController,SIGNAL(networkError()),this,SLOT(networkError()));
    connect(&video->player, SIGNAL(volumeChanged(int)),this,SLOT(volumeChanged(int)));
    connect(&video->player,SIGNAL(videoAvailableChanged(bool)),this,SLOT(videoAvailableChanged(bool)));

    connect(video,&VideoController::informationSet,[=](QString msg){
       //ui->info_label->setText(msg);
    });

    connect(&video->player,&QMediaPlayer::stateChanged,[=](QMediaPlayer::State state){
        if (state == QMediaPlayer::PlayingState){
             ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
        }else{
            ui->playBtn->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        }

    });
    //connect(videoWidget,SIGNAL(mouseDoubleClickEvent(QMouseEvent * )),this, SLOT(onDoubleClicked(QMoustEvent*)));

    connect(video,&VideoController::userListUpdated, [=](const UserList &ls){
     //  qDebug() <<ls;
         ui->tableWidget->clearContents();
        int i =0;
        ui->tableWidget->setRowCount(ls.count());
         for (User u :ls){

            ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(u.id)));
             ui->tableWidget->setItem(i,1,new QTableWidgetItem(QString::number(u.ping)));
              ui->tableWidget->setItem(i,2,new QTableWidgetItem(VideoController::getStateString(u.state)));
              i++;
        }

    });
    connect(video,&VideoController::userUpdated, [=](const User & user){
       //qDebug() <<"user ping " << user.ping;
    });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_playBtn_clicked()
{
    if (lock){
        return;
    }
    if (video->player.state() == QMediaPlayer::PlayingState){
        video->pause();
    }else{
        video->play();
    }
}


void MainWindow::on_pauseBtn_clicked()
{
    if (lock){
        return;
    }
    video->pause();
}


void MainWindow::on_timeSlider_sliderReleased()
{
    if (!lock){
        video->seekTo(ui->timeSlider->value());
    }
    pulling =false;
}

void MainWindow::on_timeSlider_sliderPressed()
{

    //video->pause();
    pulling = true;
}

void MainWindow::on_addBtn_clicked()
{
    if (lock){
        return;
    }
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
    if (!video->netController->isOnline()&&!lock){
        if (this->dialog == 0){
            dialog = new OnlineDialog(this);

        }
        dialog->show();
    }else{

        video->netController->disconnectThis();
    }
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

    if (!pulling)
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
    lock=true;
    ui->actionOnline->setText("Offline");
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

void MainWindow::showConsoleMessage(QString msg)
{
    ui->consoleDisplay->append(msg+"\n");
}

void MainWindow::online(bool host)
{
    if (host){
        setWindowTitle("online(host)");
    }else{
        setWindowTitle("online(client)");
        showConsoleMessage("wait for init");
    }
    ui->actionOnline->setText("Offline");
}

void MainWindow::offline()
{
    setWindowTitle("offline");
    ui->actionOnline->setText("Online");
}

void MainWindow::on_fullScreenBtn_clicked()
{
    videoWidget->setFullScreen(true);
}



void MainWindow::resetList()
{
    model->clear();
    this->itemMap.clear();
}

void MainWindow::clientInitComplete()
{
    showConsoleMessage("init completed");
    lock=false;
}

void MainWindow::networkError()
{
    lock=false;
    ui->actionOnline->setText("Online");
}

void MainWindow::on_volumeSpinBox_editingFinished()
{

}

void MainWindow::volumeChanged(int volume)
{
    ui->volumeSpinBox->setValue(volume);
}

void MainWindow::on_volumeSpinBox_valueChanged(int arg1)
{

    video->player.setVolume(arg1);
}

void MainWindow::videoAvailableChanged(bool able)
{
    if (able){
        ui->volumeSpinBox->setValue(video->player.volume());
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    videoWidget->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
  //  const QRect rect = ui->info_label->geometry();
  //  ui->info_label->setGeometry(0,0,ui->playArea->width(),rect.height());

    //container->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
      // qmlVideo->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
   // QObject * obj = (QObject* )qmlVideo->rootObject();
    //obj->setProperty("x",ui->playArea->width());obj->setProperty("y",ui->playArea->height());

}

void MainWindow::showEvent(QShowEvent *event)
{
     videoWidget->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
     //const QRect rect = ui->info_label->geometry();
    // ui->info_label->setGeometry(0,0,ui->playArea->width(),rect.height());
     //container->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
      //  qmlVideo->setGeometry(0,0,ui->playArea->size().width(),ui->playArea->size().height());
     //QObject * obj = (QObject* )qmlVideo->rootObject();
     //obj->setProperty("x",ui->playArea->width());obj->setProperty("y",ui->playArea->height());
\
}
