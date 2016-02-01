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
#include "jstoqtapi.h"
#include "networkcontroller.h"
#include "definition.h"
class VideoController : public QObject
{
    Q_OBJECT
private:
    QWebView * webView;
    QWebFrame * frame;

    QNetworkAccessManager qnam;
    bool webReady = false;
    //get title
    bool httpBusy= false;
    QString tempBuffer;
    QString url="";
    void titleReturned(QString title);
    void clientInit(Message &msg);
    double currentTime =0;


 //---------
    int vid = 0;
    int currentId =-1;
    int state = PAUSE;
    void getTitle(QString url);
    void setCurrentVideo(int id);

    void _play();
    void _pause();
    void _seekTo(double sec);

    void suggestPlay();
   //---online
    int currentSeq = 0;
    int onlineStatus = IDLE;
    int waitingCount =0;
    int nextCommand = 0;
public:

    JsToQtApi * api;
    NetworkController * netController;
    bool playerLoaded = false;
    double duration = 0;
    QMap<int, QPair<QString,QString> > links;
    explicit VideoController(QWebView * view, QObject *parent = 0);
    void play();
    void pause();
    void seekTo(double sec);
    void loadVideo(int id);
    void addVideo(QString url);
    void updateTime();




signals:
    void videoAdded(int vid,QString title);
    void videoOnPlay(int vid,QString title);
    void resetPlayList();

private slots:
     void attachWindowObject();
     void replyReady();
     void replyFinished();
     void videoEnded();
    void parseMessage(Message &msg);
    void displayValue(double value);
    void onLoaded(double duration);
    void youtubeApiReady();
    void helloClient(Message & msg);

};

#endif // VIDEOCONTROLLER_H
