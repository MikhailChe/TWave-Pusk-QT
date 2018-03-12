#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFile>
#include <QtGlobal>
#include <QtCore>



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()

{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP-1251"));
    QSettings set("settings.ini",QSettings::IniFormat);
    QFileDialog DIALOG;
    QString FILE = DIALOG.getExistingDirectory(this, tr("Open Directory"),
                                   set.value("Widget/DIR").toString(),
                                   QFileDialog::ShowDirsOnly
                                   | QFileDialog::DontResolveSymlinks);
    if (FILE!="") {
        set.setValue("Widget/DIR",FILE);
        QDir DIR(FILE);

        QStringList dir= DIR.entryList(QDir::Files);
        //ui->label->setText(ui->label->text()+"\r\n"+QString("%1  ").arg(DIALOG.result())+" "+FILE);

       // QTextCodec::setCodecForTr(QTextCodec::codecForName("CP-1251"));
       // QSettings set("settings.ini",QSettings::IniFormat);
        QString LOG = set.value("LOGFILE").toString();
        QString REZ = set.value("REZFILE").toString();
        for (int i=0; i<dir.size();i++)
            if ((dir[i]==LOG)||(dir[i]==REZ)) QFile::remove(DIR.absolutePath() +"/"+dir[i]);
        for (int i=0; i<dir.size();i++){

            set.setValue("FILE",DIR.absolutePath() +"/"+dir[i]);
            ui->label->setText(ui->label->text()+"\r\n"+DIR.absolutePath() +"/"+dir[i]+ "    ");
            QEventLoop loop;
                QTimer::singleShot(10, &loop, SLOT(quit()));
                loop.exec();
            QProcess pi;
            pi.start(set.value("Widget/FILE").toString());

            ui->label->setText(ui->label->text()+QString("%1").arg(pi.exitCode()));

            pi.waitForFinished();
            QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}


    }

}

void MainWindow::Connect()
{
    /**/


    /*for(;;)
    {
        if (socket.waitForConnected(30000))
        {
            socket.write("");
            socket.close();
        }

    }*/

   //


}

void MainWindow::on_pushButton_2_clicked()
{
    /*QTcpServer tcpServer ;
         if (!tcpServer.listen()) {
             QMessageBox::critical(this, tr("Fortune Server"),
                                   tr("Unable to start the server: %1.")
                                   .arg(tcpServer.errorString()));
             close();
             return;
         }
    QMessageBox::critical(this, tr("Fortune Server"),tr("The server is running on port %1.\n"
                             "Run the Fortune Client example now.")
                          .arg(tcpServer.serverPort()));*/
    /*QFile FILEDATA("021111134510.txt");
    QTcpSocket socket;
    socket.connectToHost("127.0.0.1",4567);
    quint32 sft;
    if (!FILEDATA.open(QIODevice::ReadOnly)){
        return;
    }
    QDataStream FIL(&FILEDATA);
    //FIL.setVersion(11);
    if (socket.waitForConnected(10000)){
        char block[7]="qwerty";
        //block;
        while(!FIL.atEnd()){
        //QString st ;
        FIL.readRawData(block,sizeof(block));
        sft =sizeof(block);
        //sft =st.length();
        socket.write(block,sizeof(block));
        socket.waitForBytesWritten(10000);
        socket.disconnectFromHost();
        }
    }
 socket.close();*/
    tcpSocket = new QTcpSocket(this);
    requestNewFortune();
    connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(readFortune()));

}
void MainWindow::readFortune()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    if (blockSize == 0) {
        if (tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;

        in >> blockSize;
    }

    if (tcpSocket->bytesAvailable() < blockSize)
        return;

    QString nextFortune;
    in >> nextFortune;
    ui->label_5->text()=nextFortune;
    if (nextFortune == currentFortune) {
        QTimer::singleShot(0, this, SLOT(requestNewFortune()));
        return;
    }

    currentFortune = nextFortune;


}
void MainWindow::requestNewFortune()
 {
     blockSize = 0;
     tcpSocket->abort();
     tcpSocket->connectToHost(QHostAddress("172.16.21.8"),ui->lineEdit->text().toInt());
 }
