#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
#include <QProcess>
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
    string path = pwd + "\\translation_vi.qm";
    if (translatorVi.load(QString::fromStdString(path)))
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

    ui->spinBox->setValue(6);

    TCHAR pwd_tchar[100];
    GetCurrentDirectory(100, pwd_tchar);
    wstring tmp_wstr(pwd_tchar);
    string tmp_str(tmp_wstr.begin(), tmp_wstr.end());
    pwd = tmp_str;
    loadSettings();
}

void MainWindow::test(string cmd) {
    proc = new QProcess();
    proc->setReadChannel(QProcess::StandardError);
    QString command = QString::fromStdString(cmd);

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
    string inipath = pwd + "\\setting.ini";
    QSettings setting(QString::fromStdString(inipath), QSettings::IniFormat);

    setting.beginGroup("ImageType");
    QString type = setting.value("type").toString();
    if (type == QString("Raw"))
        arg[0] = "";
    else if (type == QString("SMART"))
        arg[0] = "--s01";
    else if (type == QString("E01"))
        arg[0] = "--e01";
    else {

    }
    setting.endGroup();

    setting.beginGroup("EvidenceItemInfomation");
    arg[1] = setting.value("caseNum").toString().toStdString();
    arg[2] = setting.value("evidenceNum").toString().toStdString();
    arg[3] = setting.value("description").toString().toStdString();
    arg[4] = setting.value("examiner").toString().toStdString();
    arg[5] = setting.value("notes").toString().toStdString();
    setting.endGroup();

    setting.beginGroup("ImageDestinationSetting");
    arg[6] = setting.value("frag").toString().toStdString();
    arg[7] = setting.value("compress").toString().toStdString();
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

    string command = pwd + "\\ftkimager ";
    if (ui->comboBox_2->currentIndex() == 0)
        command += ui ->comboBox->currentText().toStdString().substr(0, 18) + " ";
    else
        command += ui ->comboBox->currentText().toStdString().substr(0, 2) + " ";
    command += ui ->lineEdit->text().toStdString();
    command += "/" + ui->lineEdit_2->text().toStdString() + " ";
    if (ui->comboBox_3->currentIndex() == 0)
        command += "--e01";
    else if (ui->comboBox_3->currentIndex() == 2)
        command += "--s01";
    if (ui->lineEdit_3->text().size() != 0)
        command += " --case-number \"" + ui->lineEdit_5->text().toStdString() + "\"";
    if (ui->lineEdit_4->text().size() != 0)
        command += " --evidence-number \"" + ui->lineEdit_5->text().toStdString() + "\"";
    if (ui->lineEdit_5->text().size() != 0)
        command += " --description \"" + ui->lineEdit_5->text().toStdString() + "\"";
    if (ui->lineEdit_6->text().size() != 0)
        command += " --examiner \"" + ui->lineEdit_6->text().toStdString() + "\"";
    if (ui->lineEdit_7->text().size() != 0)
        command += " --notes \"" + ui->lineEdit_7->text().toStdString() + "\"";
    command += " --frag " + ui->lineEdit_8->text().toStdString() + "MB";
    char tmp[2];
    itoa(ui->spinBox->value(), tmp, 10);
    command += " --compress " + string(tmp);
    qDebug() << command.c_str() << endl;
    test(command);
//    system(command.c_str());
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
