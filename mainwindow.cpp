#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QProcess>
#include <QDir>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <windows.h>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    QString path = pwd + "\\translation_vi.qm";
    if (translatorVi.load(path))
        qDebug()<<"successfully load translation_vi file.";

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent* event)
{
    if(event->type() == QEvent::LanguageChange)
    {
        ui->retranslateUi(this);
    }

    QWidget::changeEvent(event);
}

void MainWindow::init() {
    QString cmd[2];
    cmd[0] = "wmic diskdrive get DeviceID, Model";
    cmd[1] = "wmic logicaldisk get caption,volumename,description";

    for (int i = 0; i < 2; i++) {
        size[i] = 0;
        proc = new QProcess();
        proc->start(cmd[i]);
        proc->waitForFinished();
        while (1) {
            QString line = proc->readLine();
            if (line.size() == 0)
                break;
            line = line.trimmed();
            if (line.indexOf("\\\\.\\") == -1 && line.indexOf(":") == -1)
                continue;
            qDebug() << line;
            listDrive[i][size[i]] = line;
            size[i]++;
        }
        delete proc;

    }
    on_comboBox_2_activated(0);

    pwd = QDir::currentPath();
    loadSettings();
}

void MainWindow::test(QString command) {
    proc = new QProcess();
    proc->setReadChannel(QProcess::StandardError);

    connect(proc, SIGNAL(readyReadStandardError()),            this, SLOT(updateProgress()));
    connect(proc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finishProgress()));

    ui->textEdit->append(command);
    proc->start(command);
    qDebug() << proc->state();
    proc->waitForReadyRead();
    ui->textEdit->append(proc->readAllStandardError());
}

void MainWindow::updateProgress() {
    QString line = proc->readLine();
    line = line.trimmed();
    ui->textEdit->append(line);

}

void MainWindow::finishProgress() {
    delete proc;
}

void MainWindow::loadSettings() {
    QString inipath = pwd + "\\setting.ini";
    QSettings setting(inipath, QSettings::IniFormat);

    setting.beginGroup("ImageType");
    QString type = setting.value("type").toString();
    if (type == QString("Raw"))
        ui->comboBox_3->setCurrentIndex(1);
    else if (type == QString("SMART"))
        ui->comboBox_3->setCurrentIndex(2);
    else if (type == QString("E01"))
        ui->comboBox_3->setCurrentIndex(0);
    else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Image type invalid!\nCheck your configuration setting file.");
        messageBox.setFixedSize(1000,400);
        exit(-1);
    }
    setting.endGroup();

    setting.beginGroup("ImageDestinationSetting");
    QString fragValue = setting.value("frag").toString();
    bool ok;
    fragValue.toInt(&ok, 10);
    qDebug() << fragValue;
    if (ok)
        ui->lineEdit_8->setText(fragValue);
    else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Fragments size invalid!\nCheck your configuration setting file.");
        messageBox.setFixedSize(1000,400);
        exit(-1);
    }
    QString compressValue = setting.value("compress").toString();
    int tmp = compressValue.toInt(&ok, 10);
    if (ok && tmp >= 0 && tmp <= 9)
        ui->spinBox->setValue(tmp);
    else {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Compression level invalid!\nCheck your configuration setting file.");
        messageBox.setFixedSize(1000,400);
        exit(-1);
    }
    setting.endGroup();
}

void MainWindow::on_pushButton_3_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                    "",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);
    ui -> lineEdit ->setText(dir);
}

void MainWindow::on_pushButton_clicked()
{

    QString command = pwd + "\\ftkimager ";
    if (ui->comboBox_2->currentIndex() == 0)
        command += ui ->comboBox->currentText().left(18) + " ";
    else
        command += ui ->comboBox->currentText().left(2) + " ";
    QString dir = ui ->lineEdit->text();
    if (! QDir(dir).exists()) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Destination Image Folder does not exist!");
        messageBox.setFixedSize(1000,400);
        return;
    }
    command += dir;
    command += "/" + ui->lineEdit_2->text()+ " ";
    if (ui->comboBox_3->currentIndex() == 0)
        command += "--e01";
    else if (ui->comboBox_3->currentIndex() == 2)
        command += "--s01";
    if (ui->lineEdit_3->text().size() != 0)
        command += " --case-number \"" + ui->lineEdit_5->text() + "\"";
    if (ui->lineEdit_4->text().size() != 0)
        command += " --evidence-number \"" + ui->lineEdit_5->text() + "\"";
    if (ui->lineEdit_5->text().size() != 0)
        command += " --description \"" + ui->lineEdit_5->text() + "\"";
    if (ui->lineEdit_6->text().size() != 0)
        command += " --examiner \"" + ui->lineEdit_6->text() + "\"";
    if (ui->lineEdit_7->text().size() != 0)
        command += " --notes \"" + ui->lineEdit_7->text() + "\"";

    QString fragValue = ui->lineEdit_8->text();
    bool ok;
    fragValue.toInt(&ok, 10);
    if (! ok) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Fragment size invalid!\nCheck fragment size option in advanced setting tab.");
        messageBox.setFixedSize(1000,400);
        return;
    }
    command += " --frag " + fragValue + "MB";

    int comprValue = ui->spinBox->value();
    if (comprValue < 0 || comprValue > 9) {
        QMessageBox messageBox;
        messageBox.critical(0,"Error","Compression level invalid!\nCheck compression option in advanced setting tab.");
        messageBox.setFixedSize(1000,400);
        return;
    }

    QString tmp = QString::number(comprValue);
    command += " --compress " + tmp;
    qDebug() << command << endl;
    test(command);
}

void MainWindow::on_actionEnglish_triggered()
{
    qApp->removeTranslator(&translatorVi);
}

void MainWindow::on_actionVietnamese_triggered()
{
    qApp->installTranslator(&translatorVi);
}

void MainWindow::on_comboBox_2_activated(int index)
{
    ui ->comboBox->clear();
    for (int i = 0; i < size[index]; i++)
        ui ->comboBox->addItem(listDrive[index][i]);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (proc)
    {
        proc->kill();
        updateProgress();
        proc = NULL;
       }
}
