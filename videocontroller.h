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



 //---------
    int vid = 0;
    int currentId =-1;
    bool playing = false;
    void getTitle(QString url);
    void setCurrentVideo(int id);
public:

    JsToQtApi * api;
    NetworkController * netController;
    bool loaded = false;
    double duration = 0;
    QMap<int, QPair<QString,QString> > links;
    explicit VideoController(QWebView * view, QObject *parent = 0);
    void play();
    void pause();
    void seekTo(double sec);
    void loadVideo(int id);
    void addVideo(QString url);




signals:
    void videoAdded(int vid,QString title);
    void videoOnPlay(int vid,QString title);

private slots:
     void attachWindowObject();
     void replyReady();
     void replyFinished();
     void videoEnded();

    void displayValue(double value);
    void onLoaded(double duration);
    void youtubeApiReady();

};

#endif // VIDEOCONTROLLER_H
