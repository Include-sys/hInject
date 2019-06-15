#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::setWindowTitle("hInjector");
    MainWindow::setWindowIcon(QIcon(":/images/icon.png"));
    ui->pushButton_inject->setToolTip("Select Dll and Process");
    ui->statusBar->showMessage("Status: ");
    connect(this, &MainWindow::ready, this, &MainWindow::isready);


    mInject = new Injector();
    mDialog = new Dialog(this);
    mAbout = new About(this);

    connect(mDialog, &Dialog::processOpened, this, &MainWindow::isProcessOpened);


    //delete mInject;
}

MainWindow::~MainWindow()
{
    delete mInject;
    delete ui;
}

void MainWindow::on_pushButton_selectdll_clicked()
{
    QString filter = "Dynamic Link Library File (*.dll)";

    //Native File Dialog call has bugs. So using Qt File Dialog

    mInject->dllPath = QFileDialog::getOpenFileName(this, "Select a DLL", QDir::rootPath(), filter, 0);
    qDebug() << mInject->dllPath;

    if(mInject->dllPath.isEmpty()) return;

    ui->lineEdit->setText(mInject->dllPath);
    ui->statusBar->showMessage("Status: Dll Selected");
    emit ready();

}

void MainWindow::on_actionOpen_Process_triggered()
{
    mDialog->exec();
    emit ready();
}

void MainWindow::isready()
{
    if(mDialog->status && !mInject->dllPath.isEmpty())
    {
        ui->statusBar->showMessage("Status: Ready!");
        ui->pushButton_inject->setEnabled(true);
    }
}

void MainWindow::isProcessOpened(int pID)
{
    ui->statusBar->showMessage("Status: " + QString::number(pID) + " Selected");
}

void MainWindow::on_pushButton_inject_clicked()
{
    mInject->pID = mDialog->pID;
    int mStatus = mInject->Inject();

    switch(mStatus)
    {

    case Injector::E_SUCCESS:
        ui->statusBar->showMessage("Status: Successfully Injected");
        break;
    case Injector::E_OPEN_PROCESS_FAILED:
        ui->statusBar->showMessage("Status: Failed. Error: Open Process Failed");
        break;
    case Injector::E_MEM_ALLOC_FAILED:
        ui->statusBar->showMessage("Status: Failed. Error: Memory Allocation Failed.");
        break;
    case Injector::E_CREATE_REMOTE_THREAD_FAILED:
        ui->statusBar->showMessage("Status: Failed. Error: 'Create Remote Thread' Function Failed.");
    default:
        break;
    }
}

void MainWindow::on_actionAbout_triggered()
{
    mAbout->show();
}
