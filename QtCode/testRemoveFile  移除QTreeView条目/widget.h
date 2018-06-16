#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QFileSystemModel>

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
    void slot_deleteFile();

private:
    Ui::Widget *ui;

    QFileSystemModel *model;
};

#endif // WIDGET_H
