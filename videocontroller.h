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
    bool httpBusy= false;
    QString tempBuffer;
    QString url="";
    void titleReturned(QString title);
    void clientInit(Message &msg);
    double currentTime =0;
    VideoDownloader * downloader;

    QMediaPlaylist playlist;

 //---------
    int vid = 0;
    int currentId =-1;
    int state = PAUSE;
    void getTitle(QString url);
    void setCurrentVideo(int id);

    void _play();
    void _pause();
    void _seekTo(qint64 pos);

    void suggestPlay();
   //---online
    int currentSeq = 0;
    int onlineStatus = IDLE;
    int waitingCount =0;
    int nextCommand = 0;
    QTimer onlineTimer;
    void suggestPause();
    void initMessage(Message &msg);
    const int beatInterval = 2000;
    const double maxDelay = 1;
    const double bufferTime = 2;
    QString extractVid(QString url);
public:
    QMediaPlayer player;
    JsToQtApi * api;
    NetworkController * netController;
    bool playerLoaded = false;
    double duration = 0;
    QMap<int, QPair<QString,QString> > links;
    explicit VideoController(QVideoWidget * view, QObject *parent = 0);
    void play();
    void pause();
    void seekTo(qint64 sec);
    void loadVideo(int id);
    void addVideo(QString url);
    void updateTime();




signals:
    void videoAdded(int vid,QString title);
    void videoOnPlay(int vid,QString title);
    void resetPlayList();

private slots:
    void serverStarted();
     void attachWindowObject();
     void replyReady();
     void replyFinished();
     void videoEnded();
    void parseMessage(Message &msg);
    void displayValue(double value);
    void onLoaded(double duration);
    void youtubeApiReady();
    void helloClient(Message & msg);
    void heartBeat();

    void onDownloadFinish(bool downloaded, QString title);
};

#endif // VIDEOCONTROLLER_H
