#include "videodownloader.h"
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <iostream>
#include <QApplication>
VideoDownloader::VideoDownloader(QObject *parent) : QObject(parent)
{

}

void VideoDownloader::download(QString url,  int operation)
{
    /*
    if (busy){
        if (critical){
            process->terminate();
            title="";
        }else{
            return;
        }
    }*/

    if (downloadingMap.contains(url)){
        qDebug() <<"downloading";
    }
   QProcess* process = new QProcess(this);
    process->setProperty("title","");
    process->setProperty("url",url);
    process->setProperty("operation",operation);
    QString vid = extractVid(url);
    QStringList args;
    if (!QDir("videos").exists()){
        QDir().mkdir("videos");
    }
    if (QSysInfo::WindowsVersion != QSysInfo::WV_None ){
        process->setProgram(QDir::currentPath()+"/pythonBin/main.exe");
        args<<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/"<< "-f" << vid << url;
    }else {
        qDebug() << "use python";
        process->setProgram("python");
        args<<QDir::currentPath() + QString("/scripts/main") <<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/" << "-f" << vid<< url;
    }
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
    downloadingMap[url]=true;
    process->start();
}

void VideoDownloader::getTitle(QString url, int operation)
{
/*
    if (busy){
        if (critical){
            process->terminate();
            title="";
        }else{
            return;
        }
    }
    */


    QProcess* process = new QProcess(this);
    process->setProperty("title","");
    process->setProperty("url",url);
    process->setProperty("operation",operation);
    QStringList args;
    if (!QDir("videos").exists()){
        QDir().mkdir("videos");
    }
    if (QSysInfo::WindowsVersion != QSysInfo::WV_None ){
        process->setProgram(QDir::currentPath()+"/pythonBin/main.exe");
        args<<"-t"<< "1" << url;
    }else {
        process->setProgram("python");
        args<<QDir::currentPath()+QString("/scripts/main") <<"-t"<< "1" << url;
    }
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
    process->start();

}

QString VideoDownloader::extractVid(QString url)
{
    QRegExp exp("v=([^&]*)&?.*$",Qt::CaseInsensitive);
    exp.indexIn(url);

    qDebug() << exp.capturedTexts();
    return exp.capturedTexts()[1];
}

bool VideoDownloader::downloading(QString url)
{
   return downloadingMap.contains(url);
}

void VideoDownloader::onFinished(int exitCode, QProcess::ExitStatus exit)
{
    QProcess* process = (QProcess*) sender();
    QString title = process->property("title").value<QString>();
    QString url = process->property("url").value<QString>();
    int op = process->property("operation").value<int>();
    if (downloadingMap.contains(url)){
        downloadingMap.remove(url);
    }
    if (exitCode != 0){
        qDebug() <<"downloader:no download "<< title;
        emit finish(false,title,url,op);
    }else{
        qDebug() <<"downloader:successly download " << title;
        emit finish (true,title,url,op);
    }


    process->deleteLater();

}

void VideoDownloader::onStarted()
{
    qDebug() << "downloader stated";
}

void VideoDownloader::onStageChange(QProcess::ProcessState newState)
{
    qDebug() << "downloader stageChange "<< newState;

}

void VideoDownloader::onError(QProcess::ProcessError error)
{
    QProcess* process = (QProcess*) sender();
    qDebug() << "downloader error :" << process->errorString();
    QString url = process->property("url").value<QString>();
    if (downloadingMap.contains(url)){
        downloadingMap.remove(url);
    }
    process->deleteLater();
    emit errorSig(process->errorString());
}

void VideoDownloader::readyReadStandardOutput()
{
    QProcess* process = (QProcess*) sender();
    QString msg=  QString(process->readAll());
    qDebug() << msg;

    QRegExp exp("\\{title:(.*)\\}",Qt::CaseInsensitive);

    if (exp.indexIn(msg)!=-1){
     process->setProperty("title",exp.capturedTexts()[1]);
    }

   // qDebug() <<  process->readyReadStandardError();
}
