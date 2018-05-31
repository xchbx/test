#include "widget.h"
#include "ui_widget.h"
#include <QComboBox>
#include <QListView>
#include <QMessageBox>
#include <QString>

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);



    ui->tableWidget->horizontalHeader()->setSectionResizeMode (QHeaderView::Stretch);//自适应水平居中对齐
//    ui->tableWidget->verticalHeader()->setSectionResizeMode (QHeaderView::Stretch);//自适应居中对齐
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);  //单击选择一行
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); //设置只能选择一行，不能多行选中
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   //设置每行内容不可更改
    ui->tableWidget->setAlternatingRowColors(true);                        //设置隔一行变一颜色，即：一灰一白
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for(int i=0; i<6;i++)
    {
        ui->tableWidget->insertColumn(i);
        ui->tableWidget->setItem(row, i, new QTableWidgetItem(QString::number(i)));
        ui->tableWidget->item(row,i)->setTextAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    }

    row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    QStringList broadcastingTime;
    broadcastingTime<<tr("语文")<<tr("数学")<<tr("英语")<<tr("高数")
                   <<tr("政治")<<tr("历史")<<tr("地理")<<tr("生物")<<tr("化学")
                     <<tr("物理")<<tr("心理学")<<tr("逻辑学")<<tr("哲学");
    QComboBox *broadcastingTimeCombo=new QComboBox();
    broadcastingTimeCombo->setStyleSheet("QComboBox{border:1px solid gray;}"
      "QComboBox{font-size: 15px;}");
    broadcastingTimeCombo->setView(new QListView());
    broadcastingTimeCombo->addItems(broadcastingTime);
    ui->tableWidget->setCellWidget(row,0,broadcastingTimeCombo);

    QWidget *widgetGeneName=ui->tableWidget->cellWidget(row,0);
    QComboBox *GeneNameComboBox=(QComboBox*)widgetGeneName;
    QString geneStr = GeneNameComboBox->currentText();

    if(! geneStr.isEmpty())
    {
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(geneStr) );
        GeneNameComboBox->removeItem(GeneNameComboBox->currentIndex());
    }

    connect(ui->tableWidget,&QTableWidget::cellClicked,this,&Widget::slot_ShowRowCol);

    //QTableWidget 添加多个ComboBox
//    const int size = broadcastingTime.size()-2;
//    QComboBox*listBox = new QComboBox[size];
//    int i;
//    for(i=0;i<guideName.size()-2;i++)
//    {
//        int row = ui->tableWidget->rowCount();
//        ui->tableWidget->insertRow(row);
//        ui->tableWidget->setItem(row , 0 ,new QTableWidgetItem(guideName.at(i)));
//        listBox[i].addItems(InfoChannel);
//        ui->tableWidget->setCellWidget(row,1,&listBox[i]);
//        listBox[i].setCurrentIndex(i);
//    }

}

Widget::~Widget()
{
    delete ui;
}

void Widget::slot_ShowRowCol(int row, int col)
{
    QMessageBox box;
    box.setText(QString::number(row)+QString::number(col));
    box.exec();
}

void pShell()
{
    FILE *pShell = popen("df -h | awk -F' ' '{print $6}' | sed s/[[:space:]]//g", "r");
    int nSize = 100;
    char *pBuf = (char *)malloc(nSize);
    memset(pBuf, 0 ,nSize);
    fread(pBuf, nSize, 1, pShell);

    if(pBuf[nSize-1] != '\0')
    {
        perror("Disk name too long\n");
    }
    QString diskName = pBuf;
 //   printf("disk 111 %s \n",diskName.toStdString().c_str());

    QStringList list1 = diskName.split('\n');
    for(int i=0; i<list1.size();++i)
    {
        QString diskList = list1.at(i);
  //      printf("disk 111 %s \n",list1.at(i).toStdString().c_str());
        if(diskList.contains("/"))
        {
           /* ui->comboBox->addItem(diskList)*/;
        }
    }


    if(pShell != NULL)
    {
        pclose(pShell);
        pShell = NULL;
    }
}
