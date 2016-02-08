#include "videodownloader.h"
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <iostream>
#include <QApplication>
#include "definition.h"

void VideoDownloader::startTask(QProcess *p)
{
    if (runningCount < max_concurrent){
        p->start();
        runningCount++;
    }else{
        qDebug()<<"buffer process";
        buffer.append(p);
    }
}

void VideoDownloader::taskEnded()
{
    runningCount--;
    if (buffer.size()>0){
        qDebug()<<"pop process";
        QProcess * const localFirst = buffer.first();
        buffer.removeFirst();
        localFirst->start();
        runningCount++;
    }
}

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
        args<<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/"<< "-f"+vid << url;
    }else {
        qDebug() << "use python";
        process->setProgram("python");
        args<<QDir::currentPath() + QString("/scripts/main") <<"-e" <<"mp4" <<"-r" <<"360p" << "-p" << "videos/" << "-f"+vid<< url;
    }
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
    downloadingMap[url]=true;
    qDebug()<<"download url" << url;
    startTask(process);

}

void VideoDownloader::getTitle(QString url, int operation)
{


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
   startTask(process);

}

void VideoDownloader::readList(QString url, int operation)
{
    QProcess* process = new QProcess(this);
    process->setProperty("title","");
    process->setProperty("url",url);
    process->setProperty("operation",operation);
    process->setProperty("list",QStringList());
    QStringList args;
    if (!QDir("videos").exists()){
        QDir().mkdir("videos");
    }
    if (QSysInfo::WindowsVersion != QSysInfo::WV_None ){
        process->setProgram(QDir::currentPath()+"/pythonBin/main.exe");
        args<<"-l"<< "1" << url;
    }else {
        process->setProgram("python");
        args<<QDir::currentPath()+QString("/scripts/main") <<"-l"<< "1" << url;
    }
    connect(process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onFinished(int,QProcess::ExitStatus)));
    connect(process,SIGNAL(started()), this, SLOT(onStarted()));
    connect(process,SIGNAL(stateChanged(QProcess::ProcessState)), this, SLOT(onStageChange(QProcess::ProcessState)));
    connect(process,SIGNAL(error(QProcess::ProcessError)),this,SLOT(onError(QProcess::ProcessError)));
    connect(process,SIGNAL(readyRead()),this,SLOT(readyReadStandardOutput()));
    process->setArguments(args);
    startTask(process);
}

QString VideoDownloader::extractVid(QString url)
{
    QRegExp exp("v=([^&]*)&?.*$",Qt::CaseInsensitive);
    exp.indexIn(url);
   // qDebug() << exp.capturedTexts();
    return exp.capturedTexts()[1];
}

QString VideoDownloader::extractListId(QString url)
{
    QRegExp exp("list=([^&]*)&?.*$",Qt::CaseInsensitive);
    exp.indexIn(url);
   // qDebug() << exp.capturedTexts();
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
        if (op == LIST){
            QStringList sl = process->property("list").value<QStringList>();
            emit listReturn(sl);

        }else{

            qDebug() <<"downloader:successly download " << title;
         emit finish (true,title,url,op);
        }
    }

    taskEnded();
    process->deleteLater();

}

void VideoDownloader::onStarted()
{
    qDebug() << "downloader started";
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
    taskEnded();
    process->deleteLater();
    emit errorSig(process->errorString());
}

void VideoDownloader::readyReadStandardOutput()
{
    QProcess* process = (QProcess*) sender();
    QString msg=  QString(process->readAll());

    for (QString str : msg.split('\n')){
        QRegExp exp("\\{title:(.*)\\}",Qt::CaseInsensitive);

        if (exp.indexIn(str)!=-1){

             process->setProperty("title",exp.capturedTexts()[1]);
             continue;
        }

        QRegExp exp2("\\{progress:(.*)\\}",Qt::CaseInsensitive);

        if (exp2.indexIn(str)!=-1){
            QString title = process->property("title").value<QString>();
            double progress = QString(exp2.capturedTexts()[1]).toDouble();
            emit downloadProgress(title,progress);
        }
        QRegExp exp3("\\{list:(.*)\\}",Qt::CaseInsensitive);

        if (exp3.indexIn(str)!=-1){
            QStringList nl  = process->property("list").value<QStringList>();

            nl << exp3.capturedTexts()[1];
            process->setProperty("list",nl);

        }

    }
   // qDebug() <<  process->readyReadStandardError();
}
