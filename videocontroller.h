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
class VideoController : public QObject
{
    Q_OBJECT
private:
    QWebView * webView;
    QWebFrame * frame;
    QVideoWidget * videoWidget;
    QNetworkAccessManager qnam;
    bool webReady = false;
    //get title
  //  bool httpBusy= false;
    QString tempBuffer;
   // QString url="";
    void titleReturned(QString title);
    void clientInit(Message &msg);
    double currentTime =0;
    VideoDownloader * downloader;

    QMediaPlaylist playlist;
    VideoDestroyer destroyer;
 //---------
    int vid = 0;
    int currentId =-1;

    int nextVid = -1;
    void getTitle(QString url);
    void setCurrentVideo(int id);

    void _play();
    void _pause();
    void _seekTo(qint64 pos);
    void pickNextVideo();

    void suggestPlay(int clientId=-1);
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
    const qint64 maxDelay = 200;
    const int waitTime = 4000;
    QString extractVid(QString url);
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


public:
    QMediaPlayer player;
    JsToQtApi * api;
    NetworkController * netController;

    double duration = 0;
    QMap<int, QPair<QString,QString> > links;
    explicit VideoController(QVideoWidget * view, QObject *parent = 0);
    void play();
    void pause();
    void seekTo(qint64 sec);
    void loadVideo(int id);
    void addVideo(QString url);
    void updateTime();
    bool playable();




    bool videoExists(QString title);
signals:
    void videoAdded(int vid,QString title);
    void videoOnPlay(int vid,QString title);
    void resetPlayList();
    void consoleRead(QString msg);

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
    void mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onDownloadFinish(bool downloaded, QString title,QString url,  int operation);
    void downloaderError(QString err);
    void positionChanged(qint64 position);
};

#endif // VIDEOCONTROLLER_H
