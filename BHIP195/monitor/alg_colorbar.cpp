#include "alg_colorbar.h"


/* buffer definitions */
#define MININBUFS       2
#define MINOUTBUFS      0
#define MININBUFSIZE_0    (MAX_WIDTH * MAX_HEIGHT)
#define MININBUFSIZE_1    (MAX_WIDTH * 4)
#define MINOUTBUFSIZE   0

#define MAX_COLOR_BAR_NUM	8


#define MAX_WIDTH	MAX_DISPLAY_WIDTH_HD
#define MAX_HEIGHT	MAX_DISPLAY_HEIGHT_HD

#define MIN_TRAP	10 //均值最小跳变像素值

typedef struct node{
	//用户设置
	unsigned int uMaxThredValue;	// 最大像素差值阈值 默认150, 模拟彩条均值80-100 < 150 
	unsigned int uMinCorlorBarNum;	// 最小彩条数 默认3
	unsigned int uHeight;			// 帧的高度MAX_WIDTH
	unsigned int uWidth;			// 帧的宽度MAX_HEIGHT
	unsigned int uCutLeftWidth;		// 裁剪区域
	unsigned int uCutRightWidth;
	unsigned int uCutUpHeight;
	unsigned int uCutLowHeight;

	//私有变量
	unsigned char *pbyData;		//Y值地址
	unsigned int *puAverage;	//均值的存储地址
	unsigned int uAverageVal;	//计算出的均值
	unsigned int osd_count;
	OSD_RECT_INFO osd_rect[2];
}colorBarInfo;


int checkColorBarInOsd(int x, int y, colorBarInfo *pInfo)
{
	unsigned int i = 0; 

	for(i = 0; i < pInfo->osd_count; i++)
	{
		if((x>pInfo->osd_rect[i].x_start) && (x < (pInfo->osd_rect[i].x_start +pInfo->osd_rect[i].width)))
		{
			if((y>pInfo->osd_rect[i].y_start) && (y < (pInfo->osd_rect[i].y_start +pInfo->osd_rect[i].height)))
			{
				return 1;
			}
		}
	}

	return 0;
}

int checkColorBar(colorBarInfo *pInfo)
{
	unsigned int nRow,nCol;
	int nSum = 0;
	int iCountCol = 0;
	int iCountRow = 0;
	int iInterValRow = 2;   //隔行数目
	int iInterValCol = 1;   //隔列宽度
    int nColorber_cnt = 0;

	if(pInfo->uHeight >= 576)
	{
		iInterValRow = 2;   //隔行数目,针对高清
	}

	if(pInfo->uWidth >= 720)
	{
		iInterValCol = 2;   //标清1列，高清隔2列，checkAverage里会隔4列取一个值
	}

	for(nCol = pInfo->uCutLeftWidth; nCol < (pInfo->uWidth - pInfo->uCutRightWidth); nCol+= iInterValCol)
	{
		int nAverage = 0, nDiff = 0;
		iCountRow = 0;
		for(nRow = pInfo->uCutUpHeight; nRow < (pInfo->uHeight - pInfo->uCutLowHeight); nRow+=iInterValRow)
		{
			//隔行进行统计
			int nCur = 0;
			int ret = 0;
			ret = checkColorBarInOsd(nCol,nRow,pInfo);
			if(ret!= 0)
            {
				continue;
            }
			nCur = pInfo->pbyData[nRow * pInfo->uWidth + nCol];
			nAverage += nCur;
			iCountRow++;
		}

		nAverage /= iCountRow;
		pInfo->puAverage[nCol] = nAverage;

		for(nRow = pInfo->uCutUpHeight; nRow < (pInfo->uHeight - pInfo->uCutLowHeight); nRow+=iInterValRow)
		{
			int nCur = 0;
			int ret = 0;
			ret = checkColorBarInOsd(nCol,nRow,pInfo);
			if(ret != 0)
            {
				continue;
            }
			nCur = pInfo->pbyData[nRow * pInfo->uWidth + nCol];
			nDiff += abs(nCur - nAverage);
		}
		if((0 == nDiff)&&(nAverage >16))
		{
			nColorber_cnt++;
			if(nColorber_cnt > ( pInfo->uWidth - pInfo->uCutRightWidth-pInfo->uCutLeftWidth)/iInterValCol-5)
            {
				return 0;
            }
		}
		nSum += nDiff;
		iCountCol++;
		/*if(0 == iCountCol%100) //1920/2/100=10 time,节省CPU占用率
		{
			pInfo->uAverageVal = nSum /iCountCol*100/iCountRow;

			if (pInfo->uAverageVal > pInfo->uMaxThredValue)
			{
				//非彩条
				return pInfo->uAverageVal;
			}
		}*/
	}

	//@clr abnormal nAverage;
	nCol = pInfo->uCutLeftWidth;
	unsigned int nAverage;
	nAverage = pInfo->puAverage[nCol];
	for(nCol = pInfo->uCutLeftWidth; nCol < (pInfo->uWidth - pInfo->uCutRightWidth); nCol+= iInterValCol)
	{
		if(abs(nAverage - pInfo->puAverage[nCol]) >MIN_TRAP/2)
		{
			if(abs(pInfo->puAverage[nCol] - pInfo->puAverage[nCol+iInterValCol])>MIN_TRAP/2)
			{
				pInfo->puAverage[nCol] = nAverage;
			}
			nAverage = pInfo->puAverage[nCol];
		}
	}

    //printf("nSum:%d iCountCol:%d iCountRow:%d   Info H:%u W:%u LeftWidth:%u RightWidth:%u UpHeight:%u LowHeight:%u\n", nSum, iCountCol, iCountRow, 
    //    pInfo->uHeight, pInfo->uWidth, pInfo->uCutLeftWidth, pInfo->uCutRightWidth, pInfo->uCutUpHeight, pInfo->uCutLowHeight);

	pInfo->uAverageVal = nSum / iCountCol * 100 / iCountRow;

	if (pInfo->uAverageVal > pInfo->uMaxThredValue)
	{
		//非彩条
		//__D("uAverageVal = %d\r\n",pInfo->uAverageVal);
		return pInfo->uAverageVal;
	}
	
	return 0; //均值判断是彩条
}


int checkAverage(colorBarInfo *pInfo)
{
	unsigned int nCol;
	int iCount = 0;
	unsigned int nAverage;
	

	nCol = pInfo->uCutLeftWidth;
	nAverage = pInfo->puAverage[nCol];

	for(nCol = pInfo->uCutLeftWidth + 4; nCol < (pInfo->uWidth - pInfo->uCutRightWidth); nCol+= 4)
	{
		//printf("puAverage[%d] = %u\r\n",nCol,pInfo->puAverage[nCol]);

		if (abs(nAverage - pInfo->puAverage[nCol]) > MIN_TRAP)
		{
			iCount++;
			//@标准彩条，从左到右 Y值是递减的;
            //printf("Aver:%d  InfoAver:%d nCol:%d\n", nAverage, pInfo->puAverage[nCol], nCol);
			if(nAverage < pInfo->puAverage[nCol])
			{
                int nDiff = pInfo->puAverage[nCol] - nAverage;
                if(nDiff > 3)
                {
                    return 0;//@not color bar
                }
                else
                {
                    iCount--;
                }
			}
			nAverage = pInfo->puAverage[nCol];
		}
	
	}

	return iCount; 
}


int checkColorBarWidth(colorBarInfo *pInfo)
{
	unsigned int nCol;
	unsigned int nLastCol = 0;
	unsigned int nMaxColWidth = 0;
	unsigned int nMinColWidth = 0;
	int iColWidth = 0;
	unsigned int nAverage;
	

	nCol = pInfo->uCutLeftWidth;
	nAverage = pInfo->puAverage[nCol];
	
	for(nCol = pInfo->uCutLeftWidth + 4; nCol < (pInfo->uWidth - pInfo->uCutRightWidth); nCol+= 4)
	{
		if (abs(nAverage - pInfo->puAverage[nCol]) > MIN_TRAP)
		{
			if(0 != nLastCol)
			{
				iColWidth = nCol - nLastCol;
				nMaxColWidth = (iColWidth > nMaxColWidth)?(iColWidth):(nMaxColWidth);
				nMinColWidth = (iColWidth < nMinColWidth)?(iColWidth):(nMinColWidth);
			}
			nLastCol = nCol;
			nAverage = pInfo->puAverage[nCol];
		}
	}

	float ratio = (double)nMinColWidth/(double)nMaxColWidth;

	//@As stardand color bar, this ratio is great to 0.8;
	if(ratio > 0.8)
	{
		return 0;
	}else{
		return (int)(ratio*100); 
	}
}


///* process 调用的彩条判断入口*/
int isColorBar(colorBarInfo *pInfo)
{
	int iRet = 0;
	//判断每种色条的颜色是否均匀一致
	iRet = checkColorBar(pInfo);
	if (iRet > 0)
	{
		iRet = -5000 - iRet;
		return iRet; //非彩条
	}

	//判断各颜色条宽度是否均匀
	iRet = checkColorBarWidth(pInfo);
	if(0 != iRet)
	{
		iRet = -700000 - iRet;
		return iRet; //非彩条
	}
	
	 //判断颜色条数量
	iRet = checkAverage(pInfo);  
	if (iRet < pInfo->uMinCorlorBarNum || iRet > MAX_COLOR_BAR_NUM)
	{
		iRet = -60000 - iRet;
		return iRet; //非彩条
	}

	return 0;
}

/*
*  ======== IMGENC1COPY_TI_process ========
*/
/*
	返回值说明：
	-1：传入数据不全
	0： 传入数据齐全，完成检测

	输出参数值说明：colorBarCheck
	-1：传入数据不全
	-2：不是彩条
	0： 是彩条
*/
int alg_colorbar_process(ALG_COLORBAR_InArgs *pAlgcolorbar_input, ALG_COLORBAR_OutArgs *pAlgcolorbar_output)
{
    int flag = 0;
    colorBarInfo cb ;

	if(NULL == pAlgcolorbar_input)
	{
		return -1;
	}
			
	if((NULL == pAlgcolorbar_input->puAverage)||(NULL == pAlgcolorbar_input->pbyData))
	{
		return -2;
	}
		
    //设置结构体中的参数
    cb.uMaxThredValue	= pAlgcolorbar_input->uMaxThredValue;//彩条波动的范围值
    cb.uMinCorlorBarNum	= pAlgcolorbar_input->uMinCorlorBarNum;//隔行的数量
    cb.uWidth			= pAlgcolorbar_input->uWidth;//帧宽度
    cb.uHeight			= pAlgcolorbar_input->uHeight;//帧高度
    cb.uCutLeftWidth	= pAlgcolorbar_input->uCutLeftWidth;//裁剪区域
    cb.uCutRightWidth	= pAlgcolorbar_input->uCutRightWidth;
    cb.uCutLowHeight	= pAlgcolorbar_input->uCutLowHeight;
    cb.uCutUpHeight		= pAlgcolorbar_input->uCutUpHeight;

    if(cb.uMaxThredValue <= 50) 
	{//目前测试程度中的最佳阈值为150
        cb.uMaxThredValue = 150;//开始相应的初始化
    }

    if(cb.uMinCorlorBarNum <= 3) 
	{
        cb.uMinCorlorBarNum = 3;
    }

    if(cb.uHeight > MAX_HEIGHT) 
    {
        cb.uHeight = MAX_HEIGHT;
    }

    if(cb.uWidth > MAX_WIDTH) 
    {
        cb.uWidth = MAX_WIDTH;       //初始化帧的大小
    }

    if(cb.uCutLeftWidth + cb.uCutRightWidth + 100 >= cb.uWidth) 
    {
        cb.uCutLeftWidth  = 30;
        cb.uCutRightWidth = 30;
    } 

    if(cb.uCutLowHeight + cb.uCutUpHeight + 100 >= cb.uHeight) 
    {
        cb.uCutLowHeight = 30;
        cb.uCutUpHeight = 30;
    } 

    //设置结构体中指针指向的内存位置
    cb.pbyData		= (unsigned char *)pAlgcolorbar_input->pbyData;//y亮度值的二维数组的首地址传过来
    cb.puAverage	= (unsigned int *)pAlgcolorbar_input->puAverage;//相邻两个灰度值之间的差值
    cb.osd_count 	= pAlgcolorbar_input->osd_count;
    memcpy(cb.osd_rect, pAlgcolorbar_input->osd_rect, sizeof(OSD_RECT_INFO)*2);

    //调用判断函数进行处理
    flag = isColorBar(&cb);  //这里是主要处理函数了

    pAlgcolorbar_output->colorBarCheck = flag;
    pAlgcolorbar_output->colorAverage = cb.uAverageVal;

    return 0;
}


int alg_colorbar_printf_info(char *print_buf, int iCh, ALG_COLORBAR_InArgs input, ALG_COLORBAR_OutArgs output)
{
	char *p = NULL;
	p = print_buf;

	p += sprintf(p,"%-3d|%-6u|%-6u|%-8u|%-8u|%-10u|%-10u|%-8u|%-8u|%-8d|%-8d\r\n"
					,iCh
					,input.uWidth
					,input.uHeight
					,input.uCutLeftWidth
					,input.uCutRightWidth
					,input.uCutLowHeight
					,input.uCutUpHeight
					,input.uMinCorlorBarNum
					,input.uMaxThredValue
					,output.colorAverage
					,output.colorBarCheck);
					
	return (p-print_buf);
}

