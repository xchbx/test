#include "widget.h"
#include "ui_widget.h"
#include <QPushButton>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    setWindowTitle("Main Window");

    connect(ui->BtnShowSubbutton,&QPushButton::clicked,this,&Widget::ShowSubWindow);

    connect(&m_subWindow,&subWindow::showMainWindow,this,&Widget::hidSubWindow);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::hidSubWindow()
{
    m_subWindow.hide();
    show();
}

void Widget::ShowSubWindow()
{
    this->hide();
    m_subWindow.show();
}
