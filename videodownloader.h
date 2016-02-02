#ifndef VIDEODOWNLOADER_H
#define VIDEODOWNLOADER_H

#include <QObject>
#include <QProcess>

class VideoDownloader : public QObject
{
    Q_OBJECT
private :
    QProcess *process;
    bool busy = false;
    QString title = "";
    bool hasTitle =false;
public:
    explicit VideoDownloader(QObject *parent = 0);
    void download(QString url);

    bool getBusy() const;
    void setBusy(bool value);

signals:
    void finish(bool result, QString title);
public slots:
     void onFinished(int exitCode, QProcess::ExitStatus exit);
      void onStarted();
      void onStageChange(QProcess::ProcessState newState);
      void onError(QProcess::ProcessError error);
      void readyReadStandardOutput();
};

#endif // VIDEODOWNLOADER_H
