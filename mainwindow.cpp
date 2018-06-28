#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStringList>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QDebug>
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

//void MainWindow::init() {
//    DWORD mydrives = 100;
//    WCHAR lpBuffer[100];
//    DWORD test = GetLogicalDriveStrings( mydrives, lpBuffer);
//    int i = 0;
//    while (1) {
//        if (wcslen(lpBuffer + i) == 0)
//            break;
//        ui -> comboBox -> addItem(QString::fromWCharArray(lpBuffer + i));
//        i += 4;
//    }
//}
void MainWindow::init() {
    string cmd = "wmic diskdrive get DeviceID, Model";
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    stream = popen(cmd.c_str(), "r");
    if (stream) {
        while (!feof(stream))
            if (fgets(buffer, max_buffer, stream) != NULL) {
                string line(buffer);
                int pos = line.find("\\\\.\\");
                if (pos == -1)
                    continue;
                line.erase(line.length() - 1);
                ui ->comboBox->addItem(line.c_str());
            }
            pclose(stream);
    }
    TCHAR pwd_tchar[100];
    GetCurrentDirectory(100, pwd_tchar);
    wstring tmp_wstr(pwd_tchar);
    string tmp_str(tmp_wstr.begin(), tmp_wstr.end());
    pwd = tmp_str;
    loadSettings();
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
//        exit(0);
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
    ui -> lineEdit_2 ->setText(dir);
}

void MainWindow::on_pushButton_clicked()
{
    string command = pwd + "\\ftkimager ";
    QString cb = ui ->comboBox->currentText();
    command += cb.toStdString().substr(0, 18) + " ";
    QString le = ui ->lineEdit_2->text();
    command += le.toStdString();
    le = ui -> lineEdit->text();
    command += "/" + le.toStdString();
    QString option;
    option.sprintf(" %s --case-number %s --evidence-number %s --description \"%s\" --examiner \"%s\" --notes \"%s\" --frag %sMB --compress %s", arg[0].c_str(), arg[1].c_str(), arg[2].c_str(), arg[3].c_str(), arg[4].c_str(), arg[5].c_str(), arg[6].c_str(), arg[7].c_str());
    command += option.toStdString();
    cout << command << endl;
    system(command.c_str());
}

void MainWindow::on_actionEnglish_triggered()
{
    qApp->removeTranslator(&translatorVi);
}

void MainWindow::on_actionVietnamese_triggered()
{
    qApp->installTranslator(&translatorVi);
}
