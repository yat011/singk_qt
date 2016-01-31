#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "videocontroller.h"
#include <QStandardItemModel>
#include <QMap>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_playBtn_clicked();
    void onLoaded(double Duration);
    void setTime(double time);
    void videoAdded(int id,QString title);
    void videoOnPlay(int id,QString title);


    void on_pauseBtn_clicked();



    void on_timeSlider_sliderReleased();

    void on_timeSlider_sliderPressed();

    void on_addBtn_clicked();


private:
    Ui::MainWindow *ui;
    VideoController * video;
    QStandardItemModel * model;
    QMap <int,QStandardItem*>  itemMap;
};

#endif // MAINWINDOW_H
