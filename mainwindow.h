
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTranslator>
#include <QProcess>
#include <QCloseEvent>
#include <QTime>
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
    void runCommand(QString command);

protected:
    void changeEvent(QEvent*);
    void closeEvent(QCloseEvent *event);

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_actionEnglish_triggered();

    void on_actionVietnamese_triggered();

    void on_comboBox_2_activated(int index);

    void updateProgress();

    void finishProgress();

    void on_pushButton_2_clicked();

    void disableGetImage();

private:
    Ui::MainWindow *ui;
    QProcess *proc;
    QTranslator translatorVi;
    QString pwd;
    QString listDrive[2][100];
    int size[2];
    QTime time;
};

#endif // MAINWINDOW_H
