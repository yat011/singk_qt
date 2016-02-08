#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "onlinedialog.h"
#include "videocontroller.h"
#include <QStandardItemModel>
#include <QMap>
#include <QVideoWidget>
#include <QMediaPlayer>
#include <QQuickView>

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

    void resetList();
    void clientInitComplete();
    void networkError();


    void on_linkEdit_textChanged(const QString &arg1);

    void on_listAddBtn_clicked();

    void showContextMenu(const QPoint &pos);
    void playItem();
protected:
    void resizeEvent(QResizeEvent * event);
    void showEvent(QShowEvent *event);
private:
    bool lock=false;
    bool pulling = false;
    QQuickView * qmlVideo ;
    QWidget *container;
    Ui::MainWindow *ui;
    QVideoWidget * videoWidget;
    QStandardItemModel * model;
    QStandardItemModel * peerModel;
    QMap <int,QStandardItem*>  itemMap;
     OnlineDialog *dialog=0 ;
};

#endif // MAINWINDOW_H
