#include "videocontroller.h"
#include<QFile>
#include <QDebug>
#include <QNetworkReply>
#include <QNetworkCookie>
#include "jstoqtapi.h"
#include "message.h"
#include <QRegExp>
#include <iostream>
#include <QDir>
VideoController::VideoController(QVideoWidget *view, QObject *parent) : QObject(parent)
{
    videoWidget = view;

    player.setMedia(QUrl(QDir::currentPath()+"/Jay Chou 周杰倫【最長的電影 The Longest Movie】-Official Music Video.mp4"));
    player.setVideoOutput(view);

    /*
    QFile file("C:\\Users\\at\\Documents\\qt\\singk\\tmpl.html");
    if (!file.open(QIODevice::ReadOnly)){

    }
    QString str = file.readAll();
    downloader = new VideoDownloader();

    //webView->setHtml(str);

    frame= webView->page()->mainFrame();
    //webView->setUrl(QUrl("https://www.youtube.com/watch?v=UppBiK8xpeg"));


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
            */

    downloader= new VideoDownloader();
    connect(downloader,SIGNAL(finish(bool,QString)),this,SLOT(onDownloadFinish(bool,QString)));



    netController = new NetworkController();
    //network
    connect(netController,SIGNAL(newClientConnected(Message&)),this,SLOT(helloClient(Message&)));
    connect(netController,SIGNAL(messageComeIn(Message&)),this,SLOT(parseMessage(Message&)));
    connect(netController,SIGNAL(serverStarted()),this,SLOT(serverStarted()));
    connect(&onlineTimer,SIGNAL(timeout()),this,SLOT(heartBeat()));

}
void VideoController::attachWindowObject()
{
    qDebug() << "attach";
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

void VideoController::parseMessage(Message &msg)
{
    if (!netController->isHost()){
        Message reply;
        switch(msg.getType()){
        case HELLO:
            clientInit(msg);
            break;
        case PRE_PLAY:
        qDebug()<<"client: "<< currentSeq << " " << msg.getSeq();
        if (currentSeq < msg.getSeq()){
            qDebug()<<"client: pre play";
            currentSeq = msg.getSeq();
            reply.setClientId(netController->getClientId());
            reply.setSeq(currentSeq);
            reply.setType(OK);
            netController->sendToHost(reply);
        }

            break;
        case PLAY:
        if (currentSeq < msg.getSeq()){
            qDebug() << "client play";
            currentSeq = msg.getSeq();
            _play();
        }

            break;
        case PAUSE:
            if (currentSeq<msg.getSeq()){
                qDebug() << "client pause";
                currentSeq = msg.getSeq();
                _pause();
            }
        break;
        default:
            break;
        }
    }else{
        switch(msg.getType()){
        case OK:
             qDebug() << "OK";
            if (onlineStatus == WAITING && msg.getSeq()==currentSeq){
                if ((--waitingCount)==0){
                    qDebug() << "play";
                    Message reply;
                    reply.setSeq(++currentSeq);
                    reply.setType(nextCommand);
                    reply.setTimeAt(currentTime);
                    reply.setCurrentState(state);
                    netController->broadcastToClients(reply);
                    if (nextCommand == PLAY){
                        _play();
                        onlineStatus=IDLE;
                    }

                }
            }
            break;
        default:
            break;
        }

    }
}

void VideoController::displayValue(double value){
    currentTime = value;


}

void VideoController::onLoaded(double duration){

    playerLoaded = true;
    this->duration = duration;
    if (state== PLAYING){
        play();
    }
    qDebug()<< duration;
}

void VideoController::youtubeApiReady(){
    qDebug() << "ready";
    webReady =true;
}

void VideoController::helloClient(Message &msg)
{
    msg.setLinks(links);
    msg.setCurrentId(currentId);
    msg.setTimeAt(currentTime);
    msg.setCurrentState(state);
    msg.setSeq(currentSeq);

}

void VideoController::heartBeat()
{
   // qDebug() << "heart Beat";
    Message msg;
    initMessage(msg);
    netController->broadcastToClients(msg);
}



void VideoController::loadVideo(int id)
{
    setCurrentVideo(id);
   // QString arg = QString("loadVideo(\"%1\")").arg(links.find(id).value().second.toHtmlEscaped());
   // qDebug()<< arg;
   // frame->evaluateJavaScript(arg);
    player.setMedia(QUrl(QDir::currentPath()+"/videos/"+links[id].first+".mp4"));
    qDebug()<<"load";
}
void VideoController::addVideo(QString url){
    if (downloader->getBusy()){
        qDebug() << "busy";
        return;
    }
    QString id = extractVid(url);
    if (id ==""){
        qDebug() << "invalid";
        return;
    }
    this->url = url;
    downloader->download(url);
}
void VideoController::onDownloadFinish(bool downloaded, QString title){

    if (title != ""){
        qDebug()<<"title "<<title;
        if (downloaded){
            qDebug()<<"added "<<title;
            links[vid++] = QPair<QString,QString>(title,url);
            if (currentId == -1){//--if no video--
                qDebug() << "no video";
                loadVideo(vid-1);
            }else{
                qDebug() << "add to list";
                emit videoAdded(vid-1, title);
            }
        }else if (QFile::exists(QDir::currentPath() + "/videos/" + title + ".mp4")){
            qDebug()<<"added "<<title;
            links[vid++] = QPair<QString,QString>(title,url);
            if (currentId == -1){//--if no video--
                qDebug() << "no video";
                loadVideo(vid-1);
            }else{
                qDebug() << "add to list";
                emit videoAdded(vid-1, title);
            }

        }else{
            //error
        }
    }else{
        //error
    }
}

QString VideoController::extractVid(QString url){
    QRegExp exp("v=([^&]*)&?.*$",Qt::CaseInsensitive);
    exp.indexIn(url);

    qDebug() << exp.capturedTexts();
    return exp.capturedTexts()[1];
}

void VideoController::updateTime()
{
    qDebug()<<"updateTime";
    frame->evaluateJavaScript("getCurrentTime()");
    qDebug()<<"updated";

}

void VideoController::serverStarted()
{
    qDebug() << "server started";
    onlineTimer.setInterval(beatInterval);
    onlineTimer.setSingleShot(false);
    onlineTimer.start();

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

void VideoController::clientInit(Message &msg)//just connected to server--- init
{

    emit resetPlayList();
    currentSeq = msg.getSeq();
    links = msg.getLinks();
    qDebug() << "init" << links.count();
    if (links.count() >0){
        currentId = msg.getCurrentId();
        if (currentId!=-1){
            qDebug()<<"load video ";
            loadVideo(currentId);
            seekTo(msg.timeAt);
        }
        for (int vid : links.keys()){
            if (vid == currentId){
                continue;
            }
            emit videoAdded(vid,links[vid].first);
        }


    }

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

void VideoController::_play()
{

    if (player.state()==player.PausedState||player.state()==player.StoppedState){
        player.play();
        state=PLAYING;
    }
}

void VideoController::_pause()
{
    if (player.state()==player.PlayingState){
        player.pause();
        state=PAUSE;
    }
}

void VideoController::_seekTo(qint64 pos)
{

       player.setPosition(pos);

        state=PAUSE;

}

void VideoController::suggestPause(){
    if (state==PAUSE){
        return;
    }
    if (netController->isHost()){
        if(netController->clients.count()==0){
            _pause();
            return;
        }
        qDebug()<<"host: pause";
        Message msg;
        initMessage(msg);
        msg.setSeq(++currentSeq);
        msg.setType(PAUSE);
       netController->broadcastToClients(msg);
       _pause();
    }else{

    }

}

void VideoController::initMessage(Message &msg)
{
      msg.setCurrentId(currentId);
      updateTime();
      msg.setTimeAt(currentTime);
      msg.setCurrentState(state);
}

void VideoController::suggestPlay()
{
    if (state==PLAYING){
        return;
    }
    if (netController->isHost()){//pre play
        if (netController->clients.count()==0){
            _play();
            return;
        }
        qDebug()<<"pre play";
        Message msg;
        msg.setSeq(++currentSeq);
        msg.setCurrentId(currentId);
        msg.setType(PRE_PLAY);
        updateTime();
        msg.setTimeAt(currentTime);
        msg.setCurrentState(state);
        waitingCount = netController->clients.count();
        onlineStatus = WAITING;
        nextCommand=PLAY;
        netController->broadcastToClients(msg);

    }else{

    }
}
void VideoController::play(){
    if (netController->isOnline()) {

     suggestPlay();
    }else{
        _play();
    }


}
void VideoController::pause(){
    if (netController->isOnline()) {
        if (netController->isHost()){
            suggestPause();
        }else{

        }

    }else{
        _pause();
    }

}
void VideoController::seekTo(qint64 sec){
    if (netController->isOnline()) {
        if (netController->isHost()){

        }else{

        }

    }else{
        _seekTo(sec);
    }

}
