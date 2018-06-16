#include "widget.h"
#include "ui_widget.h"
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QPushButton::clicked,this,&Widget::showSubWindow);

    void (subWindow::*func)()=&subWindow::outputStr;
    void (Widget::*showSlot)() = &Widget::slot_showText;
    connect(&m_subWindow,func,this,showSlot);

    void (subWindow::*funcLp)(int,QString)=&subWindow::outputStr;
    void (Widget::*showSlotLp)(int,QString) = &Widget::slot_showText;
    connect(&m_subWindow,funcLp,this,showSlotLp);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showSubWindow()
{
    hide();
    m_subWindow.show();
}

void Widget::slot_showText()
{
    qDebug()<<"empt";
}

void Widget::slot_showText(int num, QString str)
{
    qDebug()<<num<<",  "<<str;
}

