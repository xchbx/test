#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    connect(ui->pushButton,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->pushButton_2,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->pushButton_3,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->pushButton_4,&QAbstractButton::clicked,this,&Widget::clickedButton);

    connect(ui->radioButton,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->radioButton_2,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->radioButton_3,&QAbstractButton::clicked,this,&Widget::clickedButton);
    connect(ui->radioButton_4,&QAbstractButton::clicked,this,&Widget::clickedButton);
}

void Widget::clickedButton()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    QRadioButton* radio = qobject_cast<QRadioButton*>(sender());

    if (button)
    {
        QString text = button->text();
        ui->label_2->setText(text);
    }
    else if(radio)
    {
        QString text = radio->text();
        ui->label_2->setText(text);
    }
}

Widget::~Widget()
{
    delete ui;
}
