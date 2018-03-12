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

void MainWindow::on_Button_clicked()
{
    QDir DIR(QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                     "C:\\",
                                                     QFileDialog::ShowDirsOnly
                                                     | QFileDialog::DontResolveSymlinks));

    QStringList dir= DIR.entryList(QDir::Files);

    QTextCodec::setCodecForTr(QTextCodec::codecForName("CP-1251"));
    QSettings set("settings.ini",QSettings::IniFormat);
    for (int i=0; i<dir.size();i++){
        set.setValue("FILE",DIR.absolutePath() +"/"+dir[i]);
        ui->label->setText(ui->label->text()+DIR.absolutePath() +"/"+dir[i]+ "    ");

        QProcess pi;
        pi.start("test1.exe");

        ui->label->setText(ui->label->text()+QString("%1").arg(pi.exitCode())+ "    "+"\r\n");

        pi.waitForFinished();
        QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);

    }

}
