#include "videocontroller.h"
#include<QFile>
#include <QDebug>
#include <QNetworkReply>
#include "jstoqtapi.h"
#include <QRegExp>
#include <iostream>
VideoController::VideoController(QWebView * view,QObject *parent) : QObject(parent),webView(view)
{

    QFile file("C:\\Users\\at\\Documents\\qt\\singk\\tmpl.html");
    if (!file.open(QIODevice::ReadOnly)){

    }
    QString str = file.readAll();
   //qDebug() << str;

    webView->setHtml(str);
     frame= webView->page()->mainFrame();
    api = new JsToQtApi();
    connect(frame,
                SIGNAL(javaScriptWindowObjectCleared()),
            this, SLOT(attachWindowObject()));
    connect(api,
                SIGNAL(timeChanged(double)),
            this, SLOT(displayValue(double)));
    connect(api,
                SIGNAL(loaded(double)),
            this, SLOT(onLoaded(double)));
    connect(api,
                SIGNAL(youtubeApiReady()),
            this, SLOT(youtubeApiReady()));
    connect(api,
                SIGNAL(ended()),
            this, SLOT(videoEnded()));
    netController = new NetworkController();

}
void VideoController::attachWindowObject()
{
    frame->addToJavaScriptWindowObject(QString("api"), api);
}

void VideoController::replyReady()
{
    QNetworkReply* reply =  (QNetworkReply* )sender();
    tempBuffer += QString(reply->readAll());


}

void VideoController::replyFinished()
{
    QNetworkReply* reply =  (QNetworkReply* )sender();
    if( reply->error() == 0){
        QRegExp exp("<title>(.*)</title>",Qt::CaseInsensitive);
        exp.indexIn(tempBuffer);


        this->titleReturned(exp.capturedTexts()[1]);
    }else{
        //error
         qDebug()<< "title error";
         this->httpBusy =false;
    }

    reply->deleteLater();
}

void VideoController::videoEnded()
{
    qDebug() << "ended";
    links.remove(currentId);

    if (links.size() >0){
         loadVideo(links.begin().key());

    }else{
        currentId =-1;
    }
}

void VideoController::displayValue(double value){

}

void VideoController::onLoaded(double duration){
    loaded = true;
    this->duration = duration;
    if (playing){
        play();
    }
    qDebug()<< duration;
}

void VideoController::youtubeApiReady(){
    qDebug() << "ready";
    webReady =true;
}

void VideoController::play(){
    if (loaded){
        frame->evaluateJavaScript("playVideo()");
        playing =true;
    }
}
void VideoController::pause(){
    if (loaded){
        frame->evaluateJavaScript("pauseVideo()");
         playing =false;
    }
}
void VideoController::seekTo(double sec){
    if (loaded){
        qDebug()<<sec;
        frame->evaluateJavaScript(QString("seekTo(%1)").arg(sec));
        playing = false;
    }
}

void VideoController::loadVideo(int id)
{
    setCurrentVideo(id);
    QString arg = QString("loadVideo(\"%1\")").arg(links.find(id).value().second.toHtmlEscaped());
    qDebug()<< arg;
    frame->evaluateJavaScript(arg);
}
void VideoController::addVideo(QString url){
    if (httpBusy){
        qDebug() << "busy";
        return;
    }
    getTitle(url);
}
void VideoController::titleReturned(QString title)
{
    QPair<QString,QString> * ptr= new QPair<QString,QString>(title,url);
    this->links.insert(vid++,*ptr);
    if (currentId == -1){//--if no video--
        qDebug() << "no video";
         loadVideo(vid-1);
    }else{
        qDebug() << "add to list";
        emit videoAdded(vid-1, title);
    }


    this->httpBusy =false;


}

void VideoController::getTitle(QString url){

    QUrl u(url);
    this->url = url;
    QNetworkReply * reply = qnam.get(QNetworkRequest (url));
    tempBuffer = "";
    this->httpBusy = true;
    connect(reply, SIGNAL(readyRead()),this,SLOT(replyReady()));
    connect(reply, SIGNAL(finished()),this,SLOT(replyFinished()) );

}

void VideoController::setCurrentVideo(int id)
{
    currentId = id;
    qDebug() << "set current" << currentId;
    emit videoOnPlay(id, links.find(id).value().first);
}

