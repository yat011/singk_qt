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
#include <QCoreApplication>
VideoController::VideoController(QVideoWidget *view, QObject *parent) : QObject(parent)
{


    if (QSysInfo::macVersion()!= QSysInfo::MV_None){
        qDebug() << "mac";
        QDir dir = QDir(QCoreApplication::applicationDirPath());
        dir.cd("../../..");
        qDebug() <<dir.absolutePath();
        QDir::setCurrent(dir.absolutePath());
    }else{

    }

    videoWidget = view;


    player.setVideoOutput(view);


    downloader= new VideoDownloader();
    connect(downloader,SIGNAL(finish(bool,QString,QString,int)),this,SLOT(onDownloadFinish(bool,QString,QString,int)));

    //player
    connect(&player,SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),this,SLOT(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(&player,SIGNAL(positionChanged(qint64)),this,SLOT(positionChanged(qint64)));


    netController = new NetworkController();
    //network
    connect(netController,SIGNAL(newClientConnected(Message&)),this,SLOT(helloClient(Message&)));
    connect(netController,SIGNAL(messageComeIn(Message&)),this,SLOT(parseMessage(Message&)));
    connect(netController,SIGNAL(serverStarted()),this,SLOT(serverStarted()));
    connect(&onlineTimer,SIGNAL(timeout()),this,SLOT(heartBeat()));
    connect(&waitTimer,SIGNAL(timeout()),this,SLOT(applyAction()));
    connect(downloader,SIGNAL(errorSig(QString)),this,SLOT(downloaderError(QString)));

    connect(downloader,&VideoDownloader::downloadProgress,[=](QString title,double progress){
        //progress
        if (currentId==-1){//no video
            emit informationSet("Download "+title+"("+QString::number(progress)+"%)");
            if (progress==100){
                emit informationSet("");
            }
        }else if (links[currentId].first == title){// downloading next video
            emit informationSet("Download "+title+"("+QString::number(progress)+"%)");
            if (progress==100){
                emit informationSet("");
            }
        }
    });

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
            emit consoleRead("Connected");
            break;
        case PRE_PLAY:
            replyPrePlay(msg);
            if (msg.getClientId()==-1){
                emit consoleRead("host clicks play");
            }else{
                emit consoleRead("client"+QString::number(msg.getClientId())+" clicks play");
            }
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
                if (msg.getClientId()==-1){
                    emit consoleRead("host clicks pause");
                }else{
                    emit consoleRead("client"+QString::number(msg.getClientId())+" clicks pause");
                }

            }
            break;
        case SEEK:
            if (currentSeq<msg.getSeq()){
                qDebug()<< "client seek";
                currentSeq = msg.getSeq();
                _seekTo(msg.getTimeAt());
                if (msg.getClientId()==-1){
                    emit consoleRead("host seek to "+ QString::number(msg.getTimeAt()/1000));
                }else{
                    emit consoleRead("client"+QString::number(msg.getClientId())+" seek to "+  QString::number(msg.getTimeAt()/1000));
                }
            }
        case CHANGE_TO:
            if (currentSeq<msg.getSeq()){
                qDebug()<< "client change To";
                currentSeq = msg.getSeq();
                loadVideo(msg.getCurrentId());
                 emit consoleRead("change to next Video");
            }
        break;
        case HEART_BEAT:
            if (currentSeq <= msg.getSeq()){
             //   qDebug() << "client receive heart beat";
                syncState(msg);
            }
            break;
        case ADD_VIDEO:
            if (msg.getClientId()==-1){
                emit consoleRead("host add a video");
            }else{
                emit consoleRead("client"+QString::number(msg.getClientId())+" add a video");
            }
            clientAddVideo(msg);
            break;
        case BUFFER:

            if (!videoExists(VideoDownloader::extractVid(links[msg.getOptId()].second))){
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
            if (onlineStatus == WAITING && msg.getSeq()==currentSeq){
                if ((--waitingCount)==0){
                    waitTimer.stop();
                    applyAction();
                }
            }
            break;
        case PLAY:
            emit consoleRead("client"+QString::number(msg.getClientId())+" plays");
            suggestPlay(msg.getClientId());
            break;
        case PAUSE:
            emit consoleRead("client"+QString::number(msg.getClientId())+" pauses");
            suggestPause(msg.getClientId());
            break;
        case SEEK:
            emit consoleRead("client"+QString::number(msg.getClientId())+" seek to "+  QString::number(msg.getTimeAt()/1000));
            suggestSeek(msg.getTimeAt(),msg.getClientId());
        case CHANGE_TO:

        break;
        case ADD_VIDEO:
            emit consoleRead("client"+QString::number(msg.getClientId())+" add a video");
            hostAddVideo(msg,msg.getClientId());
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
        emit consoleRead("sync video with host");
        loadVideo(currentId);
        return;
    }
    if (abs(player.position()-msg.getTimeAt())> maxDelay && playable()){
         qDebug() <<"sync time" << msg.getTimeAt() << " " << player.position();
         emit consoleRead("sync time with host");
        _seekTo(msg.getTimeAt());
    }
    if (player.state() != msg.getCurrentState()){
        qDebug() <<"sync state";
        emit consoleRead("sync player state with home");
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



void VideoController::helloClient(Message &msg)
{
    msg.setLinks(links);
    initMessage(msg);


    emit consoleRead("client"+QString::number(msg.getClientId())+" joined");
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
    if (status == player.InvalidMedia){
        qDebug() << player.errorString();
    }
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


    if (!links.contains(id)&& id != -1){
        qDebug() << "doesnt not contain video " << id;
        return;
    }else{
        setCurrentVideo(id);
        if (id == -1){
            player.setMedia(QUrl());
            return;
        }
    }

     qDebug()<<"try load "<< links[id].first;
     QString yid = VideoDownloader::extractVid(links[id].second);
     qDebug() << yid;
    if (videoExists(yid)){
        if (!downloader->downloading(links[id].second)){
            qDebug()<<"now load "<< links[id].first;
            if (QSysInfo::macVersion() > 0){
                QString path = QDir::currentPath()+"/videos/"+yid+".mp4";
                player.setMedia(QUrl::fromLocalFile(path));
            }else{
                player.setMedia(QUrl(QDir::currentPath()+"/videos/"+yid+".mp4"));
            }
            nextVid = -1;
        }else{
            qDebug() << "wait for download";
            emit consoleRead("wait for downloading");
        }
    }else{
        //download;
        qDebug() <<"video "<<links[id].first<<" not exist";
         downloadVideo(links[id].second);

    }

}
void VideoController::downloadVideo(QString url){

    if (downloader->downloading(url)){
        qDebug() << "downloader downloading";
        return;
    }

    QString id = VideoDownloader::extractVid(url);
    if (id ==""){
        qDebug() << "invalid";
        return;
    }


    downloader->download(url,BUFFER);
    emit consoleRead("downloading video");

}
void VideoController::suggestAddVideo(QString title,QString url){

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
        qDebug()<<"client want to add " <<title;
        QPair<QString,QString> temp = QPair<QString,QString>(title,url);
        Message msg;
        msg.addLink(0,temp);
        msg.setType(ADD_VIDEO);
        msg.setClientId(netController->getClientId());
        msg.setOptId(0);
        netController->sendToHost(msg);
        emit consoleRead("wait for others ready");
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
void VideoController::hostAddVideo(Message &msg,int clientId){
    links[vid] = msg.getLink(msg.getOptId());
    if (currentId == -1){//--if no video--
         qDebug() << "no video";
         loadVideo(vid);
    }else{
         qDebug() << "add to list";
         emit videoAdded(vid, links[vid].first);
    }

    Message reply;
    initMessage(reply);
    reply.setType(ADD_VIDEO);
    reply.addLink(vid,links[vid]);
    reply.setOptId(vid++);
    reply.setClientId(clientId);
    netController->broadcastToClients(reply);
}

void VideoController::applyAction()
{
    if (currentSeq == BufferMsg.getSeq()){
        qDebug() << "apply action";
        Message reply;
        initMessage(reply);
        reply.setSeq(++currentSeq);
        if (BufferMsg.getType() == PRE_PLAY){
            reply.setType(PLAY);
            netController->broadcastToClients(reply);
            _play();
            onlineStatus=IDLE;
        }
    }
}


void VideoController::addVideo(QString url){


        QString id = VideoDownloader::extractVid(url);
        if (id ==""){
            qDebug() << "invalid";
            return;
        }

        if (currentId == -1){
            downloader->download(url,ADD_VIDEO);
        }else{
            downloader->getTitle(url,ADD_VIDEO);
        }
        // wait for callback
        emit consoleRead("wait for data");

}
bool VideoController::videoExists(QString title){
    return QFile::exists(QDir::currentPath() + "/videos/" + title + ".mp4");
}

void VideoController::onDownloadFinish(bool downloaded, QString title, QString url, int operation){

   // qDebug() << title << url << operation;
    if (title != ""){
        if (operation == BUFFER){
            //do sth
            if (links.contains(currentId)){
                if (links[currentId].first == title){
                    loadVideo(currentId);
                }
            }
        }else if(operation == ADD_VIDEO){
            _addVideo(title,url);
        }else{
            //ABORT
        }
    }else{
        //error
        emit consoleRead("error: acquiring video information fail");
    }

}

void VideoController::downloaderError(QString err)
{
    emit consoleRead("Error:" + err);
}

void VideoController::positionChanged(qint64 position)
{
    if (nextVid ==-1 && (player.duration()-position) < 5000 && links.count()>1){
        //buffer and set next
        if (!netController->isOnline()){
            pickNextVideo();
            if (!videoExists(VideoDownloader::extractVid(links[nextVid].second))){
                 downloadVideo(links[nextVid].second);
            }
        }else{
            suggestBuffer();
        }
    }
}


void VideoController::_addVideo(QString title,QString url){

    if (netController->isOnline()){
        suggestAddVideo(title,url);

    }else{

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


void VideoController::updateTime()
{
   // qDebug()<<"updateTime";
    //frame->evaluateJavaScript("getCurrentTime()");
   // qDebug()<<"updated";

}

bool VideoController::playable()
{
    if (player.mediaStatus() >=3 && player.mediaStatus() <=6){
        return true;
    }else{
        return false;
    }
}

void VideoController::serverStarted()
{
    qDebug() << "server started";
    onlineTimer.setInterval(beatInterval);
    onlineTimer.setSingleShot(false);
    onlineTimer.start();

}


void VideoController::clientInit(Message &msg)//just connected to server--- init
{

    emit resetPlayList();
    currentId = -1;
    currentSeq = msg.getSeq();
    links = msg.getLinks();
    qDebug() << "init" << links.count();

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



void VideoController::setCurrentVideo(int id)
{
    if (currentId != id && currentId != -1){
        destroyer.pushVideo(VideoDownloader::extractVid(links[currentId].second));
        links.remove(currentId);
        //do garbage collection

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

        if (!videoExists(VideoDownloader::extractVid(links[nextVid].second))){
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

void VideoController::suggestPause(int clientId){
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
        msg.setClientId(clientId);
        msg.setType(PAUSE);
       netController->broadcastToClients(msg);
       _pause();
    }else{
        Message msg;
        msg.setType(PAUSE);
        msg.setClientId(netController->getClientId());
        netController->sendToHost(msg);
    }

}
void VideoController::suggestSeek(qint64 pos,int clientId){

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
        msg.setClientId(clientId);
        msg.setType(SEEK);
         netController->broadcastToClients(msg);
        _seekTo(pos);
    }else{
        Message msg;
        msg.setClientId(netController->getClientId());
        msg.setType(SEEK);
        msg.setTimeAt(pos);
        netController->sendToHost(msg);
    }

}

void VideoController::suggestPlay(int clientId)
{
    if (player.state()==player.PlayingState){
        return;
    }
    if (!playable()){
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
        msg.setClientId(clientId);
        waitingCount = netController->clients.count();
        onlineStatus = WAITING;

        waitTimer.start(waitTime);
        msg.copyTo(BufferMsg);
        netController->broadcastToClients(msg);
        emit consoleRead("wait for client ready");

    }else{
        Message msg;
        msg.setType(PLAY);
        msg.setClientId(netController->getClientId());
        netController->sendToHost(msg);
        emit consoleRead("wait for others ready");
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

         suggestPause();

    }else{
        _pause();
    }

}
void VideoController::seekTo(qint64 sec){
    if (netController->isOnline()) {

         suggestSeek(sec);

    }else{
        _seekTo(sec);
    }

}
