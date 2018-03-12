#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QTcpServer>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void Connect();

private slots:
    void on_pushButton_clicked();
    void readFortune();
    void requestNewFortune();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QTcpSocket *tcpSocket;
    quint16 blockSize;
    QString currentFortune;
};

#endif // MAINWINDOW_H
