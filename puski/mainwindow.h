#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>

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
    void on_pushButton_clicked();
    unsigned int crc_chk(char* data, unsigned char length);
    void sendFortune();

    void on_pushButton_2_clicked();



    void on_checkBox_stateChanged(int arg1);

    void on_EditStart_editingFinished();

    void on_ButtonStop_clicked();
    void write_speed();
    void delay(bool rez, unsigned int pause);





    void on_pushButton_3_clicked();

private:
    Ui::MainWindow *ui;
    QTcpServer *tcpServer;

};

#endif // MAINWINDOW_H
