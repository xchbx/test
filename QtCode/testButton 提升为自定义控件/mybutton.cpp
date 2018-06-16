#include "mybutton.h"
#include <QMessageBox>

myButton::myButton(QWidget *parent):QPushButton(parent)
{

}


void myButton::mousePressEvent(QMouseEvent * event)
{
    QMessageBox box;
    box.setText(QString::fromStdString("111 button clicked"));
    box.exec();
}
