#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QMouseEvent>
#include <QtGlobal>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lineEdit->installEventFilter(this);
    bool test = connect(this,SIGNAL(clickedLineEdit()),this,SLOT(slot_printf()));
    if(!test)
    {
        qDebug("error!\n");
    }
    else
    {
        qDebug("success!\n");
    }
}


void MainWindow::slot_printf()
{
    ui->textEdit->setText(QString::fromStdString("1111111"));
    QMessageBox msg;
    msg.setText("111111111111111  ui->lineEdit");
    msg.exec();
}


bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {

        if((watched == ui->lineEdit))
        {
            QMouseEvent *clickLineEdit = (QMouseEvent*)(event);
            if(clickLineEdit->button() == Qt::LeftButton)
            {
                emit clickedLineEdit();
            }
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

MainWindow::~MainWindow()
{
    delete ui;
}
