#ifndef MYBUTTON_H
#define MYBUTTON_H
#include <QPushButton>
#include <QMouseEvent>

class myButton  : public QPushButton
{
    Q_OBJECT
public:
    explicit myButton(QWidget *parent = 0);

protected:
    virtual void mousePressEvent(QMouseEvent * event);
};

#endif // MYBUTTON_H

