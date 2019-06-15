#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "processlist.h"

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void on_pushButton_clicked();
    void on_tableWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_2_clicked();

signals:
    void processOpened(int);

private:
    Ui::Dialog *ui;
    ProcessList *mProcList;

public:
    int pID;
    bool status = false;
};

#endif // DIALOG_H
