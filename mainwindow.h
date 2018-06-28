
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QProcess>
#include <string>
#include <QDebug>

using namespace std;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void init();
    void loadSettings();
    string GetStdoutFromCommand(string cmd);
    void test();

protected:
    void changeEvent(QEvent*);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_actionEnglish_triggered();

    void on_actionVietnamese_triggered();

    void on_comboBox_2_activated(int index);


private:
    Ui::MainWindow *ui;
    QProcess *proc;
    QTranslator translatorVi;
    string pwd;
    string arg[8];
    QString listDrive[2][100];
    int size[2];
};

#endif // MAINWINDOW_H
