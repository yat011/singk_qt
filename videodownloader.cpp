#include "videodownloader.h"
#include <QDebug>
#include <QDir>
#include <QProcess>
bool VideoDownloader::getBusy() const
{
    return busy;
}

void VideoDownloader::setBusy(bool value)
{
    busy = value;
}

VideoDownloader::VideoDownloader(QObject *parent) : QObject(parent)
{


}

void VideoDownloader::download(QString url)
{
    if (busy){
        return;
    }
    busy = true;
    title="";
    process = new QProcess(this);

    QStringList args;
    if (!QDir("videos").exists()){
        QDir().mkdir("videos");
    }

    args<<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/" << url;
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
    process->setProgram(QDir::homePath()+"/Documents/pythonBin/main.exe");
    process->start();
}

void VideoDownloader::onFinished(int exitCode, QProcess::ExitStatus exit)
{
    if (exitCode != 0){
        qDebug() <<"fail download";
        emit finish(false,title);
    }else{
        qDebug() <<"successly download";
        emit finish (true,title);
    }
    process->deleteLater();
    busy = false;
}

void VideoDownloader::onStarted()
{
    qDebug() << "stated";
}

void VideoDownloader::onStageChange(QProcess::ProcessState newState)
{
qDebug() << "stageChange";
qDebug() << newState;
}

void VideoDownloader::onError(QProcess::ProcessError error)
{
    qDebug() << "error :" << error;
    qDebug() << process->errorString();
}

void VideoDownloader::readyReadStandardOutput()
{
    QString msg=  QString(process->readAll());

    QRegExp exp("\\{title:(.*)\\}",Qt::CaseInsensitive);

    if (exp.indexIn(msg)!=-1){
     qDebug() << exp.capturedTexts();
     title= exp.capturedTexts()[1];
    }

   // qDebug() <<  process->readyReadStandardError();
}
