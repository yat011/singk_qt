#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "onlinedialog.h"
#include "videocontroller.h"
#include <QStandardItemModel>
#include <QMap>
#include <QVideoWidget>
#include <QMediaPlayer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   VideoController * video;


private slots:
    void on_playBtn_clicked();


    void videoAdded(int id,QString title);
    void videoOnPlay(int id,QString title);

    //void  mediaStateChanged(QMediaPlayer::State state);
    void positionChanged(qint64 pos);
    void durationChanged(qint64 duration);
  //  void  handlePlayerError();

    void on_pauseBtn_clicked();



    void on_timeSlider_sliderReleased();

    void on_timeSlider_sliderPressed();

    void on_addBtn_clicked();
    void on_actionOnline_triggered();


public slots:
    void showOnlineDialog();
    void hostClicked();
    void clientClicked();
    void test();
private slots:
    void showConsoleMessage(QString msg);
    void online(bool host);
    void offline();
    void on_fullScreenBtn_clicked();
    void onDoubleClicked(QMouseEvent* event);
    void resetList();

private:
    Ui::MainWindow *ui;
    QVideoWidget * videoWidget;
    QStandardItemModel * model;
    QMap <int,QStandardItem*>  itemMap;
     OnlineDialog *dialog=0 ;
};

#endif // MAINWINDOW_H
