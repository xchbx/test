#include "subwindow.h"
#include "ui_subwindow.h"
#include <QPushButton>"

subWindow::subWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subWindow)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QPushButton::clicked,this,&subWindow::mySignal);

    connect(ui->pushButton_2,&QPushButton::clicked,this,&subWindow::mySendSignal);
}

subWindow::~subWindow()
{
    delete ui;
}

void subWindow::mySignal()
{
    emit outputStr();
}

void subWindow::mySendSignal()
{
    emit outputStr(111, "QString");
}
