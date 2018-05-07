QPixmap tempPix = QPixmap("你的图片");
tempPix = pixmap.scaled(QSize(100, 100), Qt::KeepAspectRatio);

m_label->setAlignment(Qt::AlignCenter);
m_label->setPixmap(tempPix);

uploadFirmware   上传固件
uploadConfig     上传配置

pushButton_3  恢复出厂
pushButton_22  格式化

    border-radius:8px;
    background-color:#0474B0;
    
	
	fontColorBt	选择颜色	fontColor1	fontColor2	fontColor3	fontColor4
	BGColorBt	选择颜色	BGColor1	BGColor2 	BGColor3	BGColor4
	

	void MainWindow::onOSDToolButtonClicked(int id)
{
    qDebug("onOSDToolButtonClicked");
    if(m_checkfontColorBtId < OSDToolButton.size())
    {
        OSDToolButton[m_checkfontColorBtId]->setChecked(false);
    }
    m_checkfontColorBtId = id;
    m_fontColor = OSDColor[id];
    OSDToolButton[m_checkfontColorBtId]->setChecked(true);
    OSDvlueChanged();
}

void MainWindow::onOSDBGToolButtonClicked(int id)
{
    qDebug("onOSDToolButtonClicked");
    if(m_checkBGColorBtId < OSDBGToolButton.size())
    {
        OSDBGToolButton[m_checkBGColorBtId]->setChecked(false);
    }
    m_checkBGColorBtId = id;
    m_BGColor = OSDColor[id];
    OSDBGToolButton[m_checkBGColorBtId]->setChecked(true);
    OSDvlueChanged();
}
	
	void MainWindow::on_BGColorBt_clicked()
{
        QColor color = QColorDialog::getColor(Qt::blue);
        if(color.isValid())
        {
            qDebug("on_BGColorBt_clicked %s",qPrintable(color.name()));
            ui->fontColorBt->setStyleSheet(("background-color:"+color.name()));
            if(m_checkBGColorBtId < LiveCtrlToolButton.size())
            {
                LiveCtrlToolButton[m_checkBGColorBtId]->setChecked(false);
            }
            m_checkBGColorBtId = LiveCtrlToolButton.size();
            m_fontColor = QColorToInt(color);
            qDebug("on_BGColorBt_clicked %x",QColorToInt(color));
            OSDvlueChanged();
        }

}




on_BGColorBt_clicked #ff4762
on_BGColorBt_clicked fd0c
OSDvlueChanged ：0 0 0 0 50 50 2 255 64832 
[BH_COM_SET_StrOSD]-980:  nCh:0,fontFamily:0,textType:0,fontSize:2,antiColor:0,color:64832,bgColor:64780,positionType:6,point_x:50,point_y:50,tSubtitleTxet:
key value bgColor
Update Key:bgColor   value: 64780
[SETSubtitleSetup]-2293: 刷新字幕
HI_MPI_RGN_Destroy failed! s32Ret: 0xa0038005.
not REG Handle:0
[LecA7_OSD_create]-33: RGN_Create succeed! Handle:0.
*****canvasInfo.str=码率*****
face_addr=e636e0,str = 码率,font_size=48
O_o!*** [RtmpConnect] connect url:rtmp://172.17.6.200:1935/live/sub
ERROR: RTMP_Connect0, failed to connect socket. 113 (No route to host)
RTMP_Connect Error 0.0.0.0 nErr:-4
[RtmpConnect] rtmp start failed!


int m_checkfontColorBtId;
    int m_checkBGColorBtId;
	
	
	void MainWindow::on_fontColorBt_clicked()
{
        /*
        getColor是QColorDialog的静态函数，打开标准颜色对话框，返回选中的颜色，参数
        表示 对话框默认选中的颜色，isValid可以判断用户选择的颜色是否有效，用户点击取消
        的话，isValid返回false
        */
        QColor color = QColorDialog::getColor(Qt::blue);
        if(color.isValid())
        {
            //如果颜色有效的话，将colorFrame设置为选中的颜色
            qDebug("on_fontColorBt_clicked %s",qPrintable(color.name()));
            ui->fontColorBt->setStyleSheet(("background-color:"+color.name()));
            if(m_checkfontColorBtId < LiveCtrlToolButton.size())
            {
                LiveCtrlToolButton[m_checkfontColorBtId]->setChecked(false);
            }
            m_checkfontColorBtId = LiveCtrlToolButton.size();
            m_fontColor = QColorToInt(color);
            qDebug("on_fontColorBt_clicked %x",QColorToInt(color));
            OSDvlueChanged();
        }

}