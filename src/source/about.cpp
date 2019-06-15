#include "about.h"
#include "ui_about.h"
#include <QImage>
#include <QPixmap>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    QImage image(":/images/icon.png");
    ui->imageLabel->setPixmap(QPixmap::fromImage(image));
    ui->imageLabel->setScaledContents(true);
}

About::~About()
{
    delete ui;
}
