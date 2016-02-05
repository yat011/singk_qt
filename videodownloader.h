#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H

#include <QMap>
#include <QObject>
#include <QProcess>

class VideoDownloader : public QObject
{
    Q_OBJECT
private :
  //  QProcess *process;
   // bool busy = false;
   // QString title = "";
    QMap<QString,bool> downloadingMap;
public:
    explicit VideoDownloader(QObject *parent = 0);
    void download(QString url, int operation);
    void getTitle(QString url, int operation);
    static QString extractVid(QString url);
    bool downloading(QString url);
    bool downloadingSth(){
        return downloadingMap.size()>0;
    }

signals:
    void finish(bool result, QString title,QString url, int operation);
    void errorSig(QString error);
    void downloadProgress(QString title,double progress);
public slots:
     void onFinished(int exitCode, QProcess::ExitStatus exit);
      void onStarted();
      void onStageChange(QProcess::ProcessState newState);
      void onError(QProcess::ProcessError error);
      void readyReadStandardOutput();
};

#endif // VIDEODOWNLOADER_H
