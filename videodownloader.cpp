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

   QProcess* process = new QProcess(this);
    process->setProperty("title","");
    process->setProperty("url",url);
    process->setProperty("operation",operation);
    QStringList args;
    if (!QDir("videos").exists()){
        QDir().mkdir("videos");
    }
    if (QSysInfo::WindowsVersion >0 ){
        process->setProgram(QDir::currentPath()+"/pythonBin/main.exe");
        args<<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/" << url;
    }else {
        qDebug() << "use python";
        process->setProgram("python");
        args<<QDir::currentPath() + QString("/scripts/main") <<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/" << url;
    }
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
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
    if (QSysInfo::WindowsVersion >0 ){
        process->setProgram(QDir::currentPath()+"/pythonBin/main.exe");
        args<<"-t"<< "1" << url;
    }else {
        qDebug() << "use python";
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

void VideoDownloader::onFinished(int exitCode, QProcess::ExitStatus exit)
{
    QProcess* process = (QProcess*) sender();
    QString title = process->property("title").value<QString>();
    QString url = process->property("url").value<QString>();
    int op = process->property("operation").value<int>();
    if (exitCode != 0){
        qDebug() <<"fail download";
        emit finish(false,title,url,op);
    }else{
        qDebug() <<"successly download";
        emit finish (true,title,url,op);
    }
    process->deleteLater();

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
    QProcess* process = (QProcess*) sender();
    qDebug() << "error :" << error;
    qDebug() << process->errorString();
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
     qDebug() << exp.capturedTexts();
     process->setProperty("title",exp.capturedTexts()[1]);
    }

   // qDebug() <<  process->readyReadStandardError();
}
