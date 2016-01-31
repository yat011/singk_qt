#ifndef ONLINEDIALOG_H
#define ONLINEDIALOG_H

#include <QDialog>
class MainWindow;
namespace Ui {
class OnlineDialog;
}

class OnlineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OnlineDialog(MainWindow * main, QWidget *parent = 0);
    ~OnlineDialog();
    QString getPortValue();
    QString getHostValue();

private:
    Ui::OnlineDialog *ui;
};

#endif // ONLINEDIALOG_H
