#include "dialog.h"
#include "ui_dialog.h"
#include <QDebug>

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);

    connect(ui->pushButton_2, &QPushButton::clicked, this, &Dialog::reject);
    connect(ui->tableWidget, &QTableWidget::cellDoubleClicked, this, &Dialog::accept);
    connect(ui->pushButton, &QPushButton::clicked, this, &Dialog::accept);

    Dialog::setWindowTitle("hInjector - Select a Process");
    mProcList = new ProcessList();

    QList<ProcessList> mList = mProcList->enumerate();

    QListIterator<ProcessList> iter(mList);

    QStringList labels;
    labels << "Proc Name" << "PID";

//    ui->tableWidget->setHorizontalHeaderLabels(QStringList("Proc Name"));
//    ui->tableWidget->setHorizontalHeaderItem(1, new QTableWidgetItem("PID"));
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setFocusPolicy(Qt::NoFocus);
    ui->tableWidget->verticalHeader()->hide();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setRowCount(mList.count());
    ui->tableWidget->setColumnCount(2);


    for(auto i = 0; i < mList.count() - 1; ++i)
    {
        QString name = mList.value(i).name();
        QString PID = QString::number(mList.value(i).pid());

        ui->tableWidget->setItem(i,0,new QTableWidgetItem(name));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(PID));
        //ui->tableWidget->setItem(i,2,new QTableWidgetItem(iter.next().windowTitle()));
        //ui->tableWidget->setItem(3,i,new QTableWidgetItem(iter.next().commandLine()));
    }
    ui->tableWidget->setHorizontalHeaderLabels(labels);



}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::on_pushButton_clicked()
{
    this->pID = ui->tableWidget->selectedItems().value(1)->text().toInt();
    this->status = true;
    emit processOpened(this->pID);
}

void Dialog::on_tableWidget_doubleClicked(const QModelIndex &index)
{
    int row = index.row();
    this->pID = index.sibling(row, 1).data().toInt();
    this->status = true;
    emit processOpened(this->pID);
}

void Dialog::on_pushButton_2_clicked()
{
    this->status = false;
}
