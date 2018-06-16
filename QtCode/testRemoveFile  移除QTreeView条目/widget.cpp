#include "widget.h"
#include "ui_widget.h"
#include <QModelIndex>
#include <QMessageBox>
#include <QString>
#include <QDebug>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);

    model = new QFileSystemModel();
//    ui->treeView->setRootIndex(model->index("F:/BHIP195/Code/trunk/联调/Recv/"));
    model->setRootPath(QString::fromStdString("F:/desktopSave"));
     ui->treeView->setModel(model);
    ui->treeView->resizeColumnToContents(0);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slot_deleteFile()));
}

void Widget::slot_deleteFile()
{
    QModelIndex currentIndex = ui->treeView->currentIndex();
    if (!currentIndex.isValid())
    {
        QMessageBox::warning(this, tr("提示!"),tr("请选择要删除的文件"));
        return;
    }

    bool ok = false;
    if (model->fileInfo(currentIndex).isDir())
    {
        ok = model->rmdir(currentIndex);
        qDebug()<<model->filePath(currentIndex).toStdString().c_str();
        qDebug()<<"rmdir error";
    }
    else
    {
        ok = model->remove(currentIndex);
        qDebug()<<model->filePath(currentIndex).toStdString().c_str();
        qDebug()<<"remove error";
    }
    if (ok)
    {
        std::string stdstr = model->filePath(currentIndex).toStdString();
        const char * filepath = stdstr.c_str();
        qDebug("filepath :%s\n",filepath);
        QString tempStr = QString::fromStdString("rm -rf ") + QString::fromStdString(filepath);
        qDebug("filepath :%s\n",tempStr.toStdString().c_str());
    }
    else
    {
        perror("delete error:");
        QMessageBox::information(this, tr("Remove"), tr("Failed to remove %1").arg(model->fileName(currentIndex)));
    }
}

Widget::~Widget()
{
    delete ui;
}
