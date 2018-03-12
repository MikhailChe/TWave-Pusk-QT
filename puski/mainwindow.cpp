#include <QTextCodec>
#include <QDateTime>
#include <QMessageBox>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <stdio.h>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qextserialport.h"

QextSerialPort port("COM2");

unsigned char ADR = 0x01; //адрес сервопривода
//char CMD=0x01;
unsigned char CMD_READ = 0x03; // команда чтения
unsigned char CMD_WRITE = 0x06; // команда записи
//-----------------------------//
unsigned char ADR_SPEED_1 = 0x01;
unsigned char ADR_SPEED_2 = 0x09;
//-----------------------------//
unsigned char ADR_CMD_1 = 0x03; //  P3
unsigned char ADR_CMD_2 = 0x06; //    06
//-----------------------------//
unsigned char ADR_SER_1 = 0x02; //  P2
unsigned char ADR_SER_2 = 0x33; //    51
//-----------------------------//
unsigned char SPEED_1 = 0x00;
unsigned char SPEED_2 = 0x00;
//-----------------------------//
unsigned char START_1 = 0x00; // старт
unsigned char START_2 = 0x21; // Первой скорости
//-----------------------------//
unsigned char STOPED_1 = 0x00; // стоп
unsigned char STOPED_2 = 0x20; // Первой скорости
//-----------------------------//
unsigned char SERVO_START_1 = 0x00; // старт
unsigned char SERVO_START_2 = 0x01; // SERVO ON
//-----------------------------//
unsigned char SERVO_STOP_1 = 0x00; // стоп
unsigned char SERVO_STOP_2 = 0x00; // SERVO OFF
//-----------------------------//
unsigned char CRC_2 = 0x00; // обнуляем
unsigned char CRC_1 = 0x00; //           CRC
//-----------------------------//
bool start = true;
bool stop = true;
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{

    ui->setupUi(this);
    /**/ port.setBaudRate(BAUD115200); /**/
    /**/ port.setParity(PAR_NONE); /**/
    /**/ port.setDataBits(DATA_8); /**/
    /**/ port.setStopBits(STOP_2); /**/
    /**/ port.setFlowControl(FLOW_OFF); /**/
    ui->listWidget->insertItem(0, QDateTime::currentDateTime().toString());
    ui->splitter_4->setVisible(false);
    ui->ButtonStop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
    //delete port;
}

void MainWindow::on_pushButton_clicked()
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));

    port.open(QIODevice::ReadWrite);
    if (port.isOpen()) {
        ui->listWidget->insertItem(0, "Порт открыт");

        if (ui->pushButton->text() == "Пуск") {

            //ui->label->setText("Двигатель работает");

            ui->pushButton->setText("Стоп");

            ui->listWidget->insertItem(0, "Двигатель запущен");

            /*char pack[]={ADR,CMD_WRITE,ADR_SER_1,ADR_SER_2,SERVO_START_1,SERVO_START_2,CRC_2,CRC_1};
        CRC_2=char(crc_chk(pack, sizeof(pack)-2));
        CRC_1=char(crc_chk(pack, sizeof(pack)-2) >> 8);
        pack[sizeof(pack)-2]=CRC_2;
        pack[sizeof(pack)-1]=CRC_1;
        port.write(pack,sizeof(pack));

        delay(false,50);*/
            char pack[] = { ADR, CMD_WRITE, ADR_CMD_1, ADR_CMD_2, START_1, START_2, CRC_2, CRC_1 };
            /*pack[2]=ADR_CMD_1; pack[3]=ADR_CMD_2; pack[4]=START_1;
        pack[5]=START_2;*/
            CRC_2 = char(crc_chk(pack, sizeof(pack) - 2));
            CRC_1 = char(crc_chk(pack, sizeof(pack) - 2) >> 8);
            pack[sizeof(pack) - 2] = CRC_2;
            pack[sizeof(pack) - 1] = CRC_1;
            port.write(pack, sizeof(pack));
        }
        else {
            ui->pushButton->setText("Пуск");

            char pack[] = { ADR, CMD_WRITE, ADR_CMD_1, ADR_CMD_2, STOPED_1, STOPED_2, CRC_2, CRC_1 };
            CRC_2 = char(crc_chk(pack, sizeof(pack) - 2));
            CRC_1 = char(crc_chk(pack, sizeof(pack) - 2) >> 8);
            pack[sizeof(pack) - 2] = CRC_2;
            pack[sizeof(pack) - 1] = CRC_1;
            port.write(pack, sizeof(pack));

            ui->listWidget->insertItem(0, "Двигатель остановлен");
        }
        port.close();
        ui->listWidget->insertItem(0, "Порт закрыт");
    }
    else {

        ui->listWidget->insertItem(0, "Порт не удалось открыть");
    }
}

unsigned int MainWindow::crc_chk(char* data, unsigned char length)
{
    int j;

    unsigned int reg_crc = 0xFFFF;
    while (length--) {
        reg_crc ^= (unsigned char)*data++;

        for (j = 0; j < 8; j++) {
            if (reg_crc & 0x01) {
                reg_crc = (reg_crc >> 1) ^ 0xA001;
            }
            else {
                reg_crc = (reg_crc >> 1);
            }
        }
    }
    return reg_crc;
}

void MainWindow::on_pushButton_2_clicked()
{
    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP-1251"));
    QSettings set("settings.ini", QSettings::IniFormat);
    QProcess pi;

    if (ui->checkBox->checkState() == 0) {
        write_speed();
        delay(true, 10);
        set.setValue("CHIS", ui->lineEdit->text().toInt() * set.value("KOOF").toUInt());
        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();
        //pi.start("test.exe");
    }
    if (ui->checkBox->checkState() == 2) {
        ui->ButtonStop->setEnabled(true);
        ui->lineEdit->setText(ui->EditStart->text());
        ui->listWidget->insertItem(0, "Запущен режим мультискорости");
        int freq = ui->EditStart->text().toInt();
        do {

            ui->lineEdit->setText(QString("%1").arg(freq));
            write_speed();
            delay(true, ui->EditSleep->text().toInt());

            if ((freq <= ui->EditStop->text().toInt()) || (!stop)) {
                //ui->lineEdit->setText(QString("%1").arg(freq));
                set.setValue("CHIS", freq * set.value("KOOF").toUInt());
                QEventLoop loop;
                QTimer::singleShot(100, &loop, SLOT(quit()));
                loop.exec();
                pi.start("test.exe");
                freq += ui->EditStep->text().toInt();
            }
            else
                stop = false;

        } while (stop);
        stop = true;
        ui->ButtonStop->setEnabled(false);
        ui->listWidget->insertItem(0, "Мультирежим скорости остановлен");
    }
}

void MainWindow::on_checkBox_stateChanged(int arg1)
{
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("CP1251"));
    if (arg1 == 2) {
        ui->splitter_4->setVisible(true);
        ui->pushButton_2->setText("Мультирежим измерений");
        ui->lineEdit->setEnabled(false);
        ui->lineEdit->setText(ui->EditStart->text());
    }
    if (arg1 == 0) {
        ui->splitter_4->setVisible(false);
        ui->pushButton_2->setText("Записать скорость");
        ui->lineEdit->setEnabled(true);
    }
}

void MainWindow::on_EditStart_editingFinished()
{
    ui->lineEdit->setText(ui->EditStart->text());
}

void MainWindow::on_ButtonStop_clicked()
{
    stop = false;
    ui->ButtonStop->setEnabled(false);
}
void MainWindow::write_speed()
{
    unsigned int freq = ui->lineEdit->text().toInt() * 30;
    SPEED_2 = char(freq);
    SPEED_1 = char(freq >> 8);

    //if ((ui->checkBox->checkState()==0)||(multifreq)) {

    port.open(QIODevice::ReadWrite);
    if (port.isOpen()) {
        char pack[] = { ADR, CMD_WRITE, ADR_SPEED_1, ADR_SPEED_2, SPEED_1, SPEED_2, CRC_2, CRC_1 };
        CRC_2 = char(crc_chk(pack, sizeof(pack) - 2));
        CRC_1 = char(crc_chk(pack, sizeof(pack) - 2) >> 8);
        pack[sizeof(pack) - 2] = CRC_2;
        pack[sizeof(pack) - 1] = CRC_1;
        port.write(pack, sizeof(pack));

        delay(false, 50);

        QByteArray resultat = port.readAll();
        if (!resultat.isNull()) {
            SPEED_2 = (char)resultat[5];
            SPEED_1 = (char)resultat[4];
            freq = ((unsigned char)SPEED_1 << 8) + (unsigned char)SPEED_2;

            ui->listWidget->insertItem(0, QString("Установлена скорость %1 Гц").arg(freq / 30));
            port.close();
        }
        else
            ui->listWidget->insertItem(0, "Данные не были записаны");
    }
}
void MainWindow::delay(bool rez, unsigned int pause)
{
    int popr = 0;
    QTime d = QTime::currentTime();
    QTime dd = QTime::currentTime();
    if (rez)
        while ((unsigned int)abs(dd.second() + popr - d.second()) < pause) {
            dd = QTime::currentTime();
            if (dd.second() < d.second())
                popr = 60;
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
    else
        while ((unsigned int)abs(dd.msec() + popr - d.msec()) < pause) {
            dd = QTime::currentTime();
            if (dd.msec() < d.msec())
                popr = 100;
            QCoreApplication::processEvents(QEventLoop::AllEvents);
        }
}

void MainWindow::on_pushButton_3_clicked()
{
    /*QFile FILEDATA("021111134510.txt");
    QTcpSocket socket;
    socket.connectToHost("172.16.21.8",5678);
    quint32 sft;
    if (!FILEDATA.open(QIODevice::ReadOnly)){
      return;
    }
    QDataStream FIL(&FILEDATA);
    //FIL.setVersion(11);
    if (socket.waitForConnected(10000)){
      char block[7]="";
      //block;
      while(!FIL.atEnd()){
      //QString st ;
      //FIL.readRawData(block,sizeof(block));
      sft =sizeof(block);
      //sft =st.length();
      //socket.readyRead();
      //socket.waitForConnected(10000);
      //socket.waitForReadyRead(5000);
      socket.read(block,sizeof(block));
      //socket.waitForBytesWritten(5000);
      ui->listWidget->insertItem(0,block);
      //if (!socket.w) return;
      }
    }
    socket.close();*/
    tcpServer = new QTcpServer(this);

    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("Fortune Server"),
            tr("Unable to start the server: %1.")
                .arg(tcpServer->errorString()));
        close();
        return;
    }
    ui->listWidget->insertItem(0, QString("Порт %1").arg(tcpServer->serverPort()));
    connect(tcpServer, SIGNAL(newConnection()), this, SLOT(sendFortune()));
}
void MainWindow::sendFortune()
{
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_4_0);
    out << (quint16)0;
    out << "abcde";
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QTcpSocket* clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
        clientConnection, SLOT(deleteLater()));

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
}
