#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void clickedLineEdit();

private slots:
    void slot_printf();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
