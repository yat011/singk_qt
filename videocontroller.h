#ifndef VIDEOCONTROLLER_H
#define VIDEOCONTROLLER_H
#include <QtWebKitWidgets/QWebView>
#include <QtWebKitWidgets/QWebFrame>
#include <QtWebKitWidgets/QWebPage>
#include <QObject>
#include <QDebug>
#include <QPair>
#include <QList>
#include <QMap>
#include <QTimer>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QMediaPlayList>
#include "jstoqtapi.h"
#include "networkcontroller.h"
#include "definition.h"
#include "videodownloader.h"
#include "videodestroyer.h"
#include "videoplayer.h"
#include <QQuickView>
class VideoController : public QObject
{
    Q_OBJECT
private:


    QQuickView * qmlView;
    QObject * qmlVideo;
    QNetworkAccessManager qnam;
    bool webReady = false;
    //get title
  //  bool httpBusy= false;
    QString tempBuffer;
   // QString url="";
    void titleReturned(QString title);
    void clientInit(Message &msg);
    double currentTime =0;



    VideoDestroyer destroyer;
 //---------
    int vid = 0;
    int currentId =-1;
    int nextVid = -1;

    void getTitle(QString url);
    void setCurrentVideo(int id);

    void _play();
    void _stop();
    void _pause();
    void _seekTo(qint64 pos);
    void pickNextVideo();


   //---- http related
    //int httpOperation = NONE;

   //---online
    int currentSeq = 0;
    int onlineStatus = IDLE;
    int waitingCount =0;
  //  int nextCommand = 0;
    Message BufferMsg ;
    QTimer onlineTimer;
    QTimer waitTimer;
    void suggestPause(int clientId=-1);
    void initMessage(Message &msg);
    const int beatInterval = 2000;
    qint64 maxDelay = 500;
    int waitTime = 4000;
    int bufferTime =10000;
    void _addVideo(QString title, QString url);
    void downloadVideo(QString url);
    bool playerLoaded = false;
    bool videoLoaded = false;
    void replyPrePlay(Message& msg);
    void suggestSeek(qint64 sec,int clientId=-1);
    void syncState(const Message &msg);
    void suggestNext();
    void suggestAddVideo(QString title, QString url);
    void clientAddVideo(Message &msg);
    void suggestBuffer();
    bool clientControllable = true;
    void hostAddVideo(Message &msg, int clientId);
    QMap<int,User> userMap;
    void replyHeartBeat(Message &msg);
    void updateUserList(UserList ls);
    int getUserState();
    User getMyUser();
    //host

    void addUser(const Message & msg);
    void updateUser(const Message &msg);
    void updateUser(const User &u);
    void removeUser(int id);

    void suggestRandom(bool r);
public:
    VideoPlayer *player;
    NetworkController * netController;
    VideoDownloader * downloader;
    double duration = 0;
    QMap<int, QPair<QString,QString> > links;
    explicit VideoController(QQuickView * view, QObject *parent = 0);

    void pause();
    void seekTo(qint64 sec);
    void loadVideo(int id);
    void addVideo(QString url);
    void updateTime();
    bool playable();
    QUrl getCurrentPath();
    static QString getStateString(int code){
        switch(code){
        case 2:
            return "Paused";
           case 1:
            return "Playing";
        case 0:
            return "Stopped";
        case UNKNOWN:
            return "unknown";
        case DOWNLOADING:
            return "Downloading";
        case NO_MEDIA:
            return "no media";
        case EndOfMedia:
            return "end of media";
        default:
            return "unknown";
        }

    }



    bool videoExists(QString title);
    void nextVideo();
    void setRandom(bool r);
    void addVideoFromList(QString url);
signals:
    void videoAdded(int vid,QString title);
    void videoOnPlay(int vid,QString title);
    void resetPlayList();
    void consoleRead(QString msg);
    void informationSet(QString msg);
    void userListUpdated(const UserList& ls);
    void userUpdated(const User& u);
    void userRemoved(int id);
public slots:
     void play();
private slots:
    void serverStarted();
    void applyAction();
    void videoEnded();
    void parseMessage(Message &msg);
    void displayValue(double value);
    void onLoaded(double duration);
    void helloClient(Message & msg);
    void heartBeat();
    void stateChanged(QMediaPlayer::State state);
    void mediaStatusChanged(int status);
    void onDownloadFinish(bool downloaded, QString title,QString url,  int operation);
    void downloaderError(QString err);
    void positionChanged(qint64 position);
    void suggestPlay(int clientId=-1);
};

#endif // VIDEOCONTROLLER_H
