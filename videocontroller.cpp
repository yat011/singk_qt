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

    //player.setMedia(QUrl(QDir::currentPath()+"/Jay Chou 周杰倫【最長的電影 The Longest Movie】-Official Music Video.mp4"));
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

    //player
    connect(&player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(&player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));


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

    if (!netController->isOnline()){


          if (nextVid!=-1){
              loadVideo(nextVid);
         }else{
              pickNextVideo();
              loadVideo(nextVid);
         }

    }else{

        if (netController->isHost()){
             suggestNext();

        }


    }
}
void VideoController::replyPrePlay( Message &msg){
    Message reply;
    qDebug()<<"client: "<< currentSeq << " " << msg.getSeq();

    if (currentSeq < msg.getSeq()){
        qDebug()<<"client: pre play";
        if(player.mediaStatus() >=3 && player.mediaStatus() <=6){
            currentSeq = msg.getSeq();
            reply.setClientId(netController->getClientId());
            reply.setSeq(currentSeq);
            reply.setType(OK);
            netController->sendToHost(reply);
       }else{//beffer Msg
            qDebug() <<"notready";
            msg.copyTo(this->BufferMsg);
        }
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
            replyPrePlay(msg);
            break;
        case PLAY:
        if (currentSeq < msg.getSeq()){
            qDebug() << "client play";
            currentSeq = msg.getSeq();
            _play();
            //syncState(msg);
        }

            break;
        case PAUSE:
            if (currentSeq<msg.getSeq()){
                qDebug() << "client pause";
                currentSeq = msg.getSeq();
                _pause();
                //syncState(msg);

            }
            break;
        case SEEK:
            if (currentSeq<msg.getSeq()){
                qDebug()<< "client seek";
                currentSeq = msg.getSeq();
                _seekTo(msg.getTimeAt());
                //syncState(msg);
            }
        case CHANGE_TO:
            if (currentSeq<msg.getSeq()){
                qDebug()<< "client change To";
                currentSeq = msg.getSeq();
                loadVideo(msg.getCurrentId());
            }
        break;
        case HEART_BEAT:
            if (currentSeq <= msg.getSeq()){
             //   qDebug() << "client receive heart beat";
                syncState(msg);
            }
            break;
        case ADD_VIDEO:
            clientAddVideo(msg);
            break;
        case BUFFER:

            if (!videoExists(links[msg.getOptId()].first)){
                qDebug() << "client buffer " << links[msg.getOptId()].first;
                 downloadVideo(links[msg.getOptId()].second);
            }
        default:
            break;
        }
    }else{
        switch(msg.getType()){
        case OK:
             qDebug() << "OK";
             qDebug() << "waiting" << waitingCount;
             qDebug() << " seq: " << msg.getSeq() << currentSeq;
            if (onlineStatus == WAITING && msg.getSeq()==currentSeq){
                if ((--waitingCount)==0){
                    qDebug() << "play";
                    Message reply;
                    initMessage(reply);
                    reply.setSeq(++currentSeq);
                    reply.setType(nextCommand);
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
void VideoController::syncState(const Message &msg){
    //check if sync
    if (currentId != msg.getCurrentId()){
        qDebug() <<"sync current";
        loadVideo(currentId);
        return;
    }
    if (abs(player.position()-msg.getTimeAt())> maxDelay ){
         qDebug() <<"sync time";
        _seekTo(msg.getTimeAt());
    }
    if (player.state() != msg.getCurrentState()){
        qDebug() <<"sync state";
        if (msg.getCurrentState() == player.PausedState){
            _pause();
            _seekTo(msg.getTimeAt());
        }else if (msg.getCurrentState() == player.PlayingState){
            _play();
            _seekTo(msg.getTimeAt());
        }else if (msg.getCurrentId() == player.StoppedState){
            player.stop();
        }

    }

}

void VideoController::displayValue(double value){
    currentTime = value;


}

void VideoController::onLoaded(double duration){

    /*
    playerLoaded = true;
    this->duration = duration;
    if (state== PLAYING){
        play();
    }
    qDebug()<< duration;
    */

}

void VideoController::youtubeApiReady(){
    qDebug() << "ready";
    webReady =true;
}

void VideoController::helloClient(Message &msg)
{
    msg.setLinks(links);
    msg.setCurrentId(currentId);
    msg.setTimeAt(player.position());
    msg.setCurrentState(player.state());
    msg.setSeq(currentSeq);

}

void VideoController::heartBeat()
{
   // qDebug() << "heart Beat";
    Message msg;
    initMessage(msg);
    msg.setType(HEART_BEAT);
    netController->broadcastToClients(msg);
}

void VideoController::stateChanged(QMediaPlayer::State state)
{

}

void VideoController::mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    qDebug()<<"media status " << status;
    if (status == player.EndOfMedia){
        videoEnded();
    }else if (status >=3 && status <=6){
        if (netController->isOnline() && !netController->isHost()){
            if (BufferMsg.getSeq() > currentSeq){
                if (BufferMsg.getType() == PRE_PLAY){
                    //reply ok
                    replyPrePlay(BufferMsg);
                }
            }
        }

    }
    if (status == player.LoadedMedia){
        if (netController->isOnline()){
            if (netController->isHost()){
                play();
            }
        }else{
            play();
        }
    }

}



void VideoController::loadVideo(int id)
{

    if (!links.contains(id)){
        qDebug() << "doesnt not contain video " << id;
        return;
    }


    setCurrentVideo(id);
    if (id == -1){
        return;
    }

     qDebug()<<"try load";
    if (videoExists(links[id].first)){
        player.setMedia(QUrl(QDir::currentPath()+"/videos/"+links[id].first+".mp4"));
        nextVid = -1;
    }else{
        //download;
        if (httpOperation != BUFFER){
            downloadVideo(links[id].second);
        }
    }

}
void VideoController::downloadVideo(QString url){
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
    httpOperation=BUFFER;
    downloader->download(url);

}
void VideoController::suggestAddVideo(QString title){

    if (netController->isHost()){

         qDebug()<<"added "<<title;
         QPair<QString,QString> temp = QPair<QString,QString>(title,url);
         Message msg;
         initMessage(msg);
         msg.addLink(vid,temp);
         msg.setType(ADD_VIDEO);
         msg.setOptId(vid);
         netController->broadcastToClients(msg);
         links[vid++] = QPair<QString,QString>(title,url);
         if (currentId == -1){//--if no video--
              qDebug() << "no video";
              loadVideo(vid-1);
         }else{
              qDebug() << "add to list";
              emit videoAdded(vid-1, title);
         }
    }else{

    }


}
void VideoController::clientAddVideo(Message &msg){

    links[msg.getOptId()] = msg.getLink(msg.getOptId());
    qDebug() << "client add " << links[msg.getOptId()].first;
    if (currentId == -1){//--if no video--
         qDebug() << "no video";
         loadVideo(msg.getOptId());
    }else{
         qDebug() << "add to list";
         emit videoAdded(msg.getOptId(), links[msg.getOptId()].first);
    }

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
        httpOperation=ADD_VIDEO;
        if (currentId == -1){
            downloader->download(url);
        }else{
            downloader->getTitle(url);
        }
        // wait for callback

}
bool VideoController::videoExists(QString title){
    return QFile::exists(QDir::currentPath() + "/videos/" + title + ".mp4");
}

void VideoController::onDownloadFinish(bool downloaded, QString title){

    if (title != ""){
        if (httpOperation == BUFFER){
            //do sth
            if (links.contains(currentId)){
                if (links[currentId].first == title){
                    loadVideo(currentId);
                }
            }
        }else if(httpOperation == ADD_VIDEO){
            _addVideo(title);
        }else{
            //ABORT
        }
    }else{
        //error
    }
    httpOperation=NONE;
}

void VideoController::positionChanged(qint64 position)
{
    if (nextVid ==-1 && (player.duration()-position) < 5000 && links.count()>1){
        //buffer and set next
        if (!netController->isOnline()){
            pickNextVideo();
            if (!videoExists(links[nextVid].first)){
                 downloadVideo(links[nextVid].second);
            }
        }else{
            suggestBuffer();
        }
    }
}


void VideoController::_addVideo(QString title){

    if (netController->isOnline()){
        suggestAddVideo(title);

    }else{

        qDebug()<<"title "<<title;

         qDebug()<<"added "<<title;
         links[vid++] = QPair<QString,QString>(title,url);
         if (currentId == -1){//--if no video--
              qDebug() << "no video";
              loadVideo(vid-1);
         }else{
              qDebug() << "add to list";
              emit videoAdded(vid-1, title);
         }

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
   // qDebug()<<"updateTime";
    //frame->evaluateJavaScript("getCurrentTime()");
   // qDebug()<<"updated";

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
    QDir::setCurrent("client");
    currentSeq = msg.getSeq();
    links = msg.getLinks();
    qDebug() << "init" << links.count();
    httpOperation=NONE;
    if (links.count() >0){
        currentId = msg.getCurrentId();
        if (currentId!=-1){
            qDebug()<<"load video ";
            loadVideo(currentId);

        }
        for (int vid : links.keys()){
            if (vid == currentId){
                continue;
            }
            emit videoAdded(vid,links[vid].first);
        }
    }
    syncState(msg);
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
    if (currentId != id && currentId != -1){
        links.remove(currentId);
        currentId=-1;
    }
    currentId = id;
    qDebug() << "set current" << currentId;
    if (id != -1){
         emit videoOnPlay(id, links.find(id).value().first);
    }else{
        emit videoOnPlay(id,"");
    }
}

void VideoController::_play()
{

    if (player.state()==player.PausedState||player.state()==player.StoppedState){
        player.play();

    }
}

void VideoController::_pause()
{
    if (player.state()==player.PlayingState){
        player.pause();

    }
}

void VideoController::_seekTo(qint64 pos)
{

       player.setPosition(pos);



}

void VideoController::pickNextVideo()
{
    if (nextVid!=-1){
        qDebug() << "next Video already picked";
        return;
    }
    for (int key :links.keys()){
        if (key != currentId){
            nextVid = key;
            return;
        }
    }
    nextVid =-1;
}
void VideoController::initMessage(Message &msg)
{ // for host
      msg.setCurrentId(currentId);
      msg.setSeq(currentSeq);
      msg.setTimeAt(player.position());
      msg.setCurrentState(player.state());
}
void VideoController::suggestBuffer(){
    if (netController->isHost()){
        qDebug() <<"host buffer";
        pickNextVideo();
        Message msg;
        initMessage(msg);
        msg.setOptId(nextVid);
        msg.setType(BUFFER);
        netController->broadcastToClients(msg);

        if (!videoExists(links[nextVid].first)){
             downloadVideo(links[nextVid].second);
        }
    }


}

void VideoController::suggestNext(){
   if (netController->isHost()){

             if (nextVid!=-1){
                   loadVideo(nextVid);
                }else{
                    pickNextVideo();
                    loadVideo(nextVid);
             }

            Message msg;
            initMessage(msg);
            msg.setType(CHANGE_TO);
            msg.setSeq(++currentSeq);
            netController->broadcastToClients(msg);
           // suggestPlay();
   }

}

void VideoController::suggestPause(){
    if (player.state()==player.PausedState){
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
void VideoController::suggestSeek(qint64 pos){

    if (netController->isHost()){
        if(netController->clients.count()==0){
            _seekTo(pos);
            return;
        }
        qDebug()<<"host: seek";
        Message msg;
        initMessage(msg);
        msg.setSeq(++currentSeq);
        msg.setTimeAt(pos);
        msg.setType(SEEK);
         netController->broadcastToClients(msg);
        _seekTo(pos);
    }else{

    }

}

void VideoController::suggestPlay()
{
    if (player.state()==player.PlayingState){
        return;
    }
    if (netController->isHost()){//pre play
        if (netController->clients.count()==0){
            _play();
            return;
        }
        qDebug()<<"pre play";
        Message msg;
        initMessage(msg);
        msg.setSeq(++currentSeq);
        msg.setType(PRE_PLAY); 
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
            suggestSeek(sec);
        }else{

        }

    }else{
        _seekTo(sec);
    }

}
