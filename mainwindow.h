#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "injector.h"
#include "dialog.h"
#include "about.h"


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
    void on_pushButton_selectdll_clicked();
    void on_actionOpen_Process_triggered();
    void isready();
    void isProcessOpened(int pID);

    void on_pushButton_inject_clicked();

    void on_actionAbout_triggered();

signals:
    bool ready();

private:
    Ui::MainWindow *ui;
    Injector *mInject;
    Dialog *mDialog;
    About *mAbout;
};

#endif // MAINWINDOW_H
