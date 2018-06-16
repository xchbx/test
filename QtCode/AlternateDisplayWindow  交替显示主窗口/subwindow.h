#ifndef SUBWINDOW_H
#define SUBWINDOW_H

#include <QWidget>

namespace Ui {
class subWindow;
}

class subWindow : public QWidget
{
    Q_OBJECT

public:
    explicit subWindow(QWidget *parent = 0);
    ~subWindow();

signals:
    void showMainWindow();

private slots:
    void hidSubWindow();

private:
    Ui::subWindow *ui;
};

#endif // SUBWINDOW_H
