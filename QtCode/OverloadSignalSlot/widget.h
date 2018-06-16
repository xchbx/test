#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "subwindow.h"

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private slots:
    void showSubWindow();

    void slot_showText();

    void slot_showText(int, QString);

private:
    Ui::Widget *ui;

    subWindow m_subWindow;
};

#endif // WIDGET_H
