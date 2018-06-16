#include "subwindow.h"
#include "ui_subwindow.h"
#include <QPushButton>

subWindow::subWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::subWindow)
{
    ui->setupUi(this);

    setWindowTitle("Sub Window");

    connect(ui->BtnToMainWindow,&QPushButton::clicked,this,&subWindow::hidSubWindow);
}

subWindow::~subWindow()
{
    delete ui;
}

void subWindow::hidSubWindow()
{
    emit showMainWindow();
}
