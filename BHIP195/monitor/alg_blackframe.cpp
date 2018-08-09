#include "alg_blackframe.h"

#define DEF_IMAGE_MARGIN			30


// 标志位设置 清除函数
#define SET_BIT(val, bitIndex)		((val) | (1 << (bitIndex)))		
#define ClEAR_BIT(val, bitIndex)	((val) & (~(1 << (bitIndex))))

/* buffer definitions */
#define MININBUFS       1
#define MINOUTBUFS      1
#define MININBUFSIZE    1
#define MINOUTBUFSIZE   1


int checkBlackFrameInOsd(int x, int y, ALG_BLACKFRAME_InArgs *pInfo)
{
	int i = 0; 
	for(i = 0; i < pInfo->osd_count; i++)
	{		
		if((x >= pInfo->osd_rect[i].x_start) && (x <= (pInfo->osd_rect[i].x_start +pInfo->osd_rect[i].width)))
		{
			if((y >= pInfo->osd_rect[i].y_start) && (y <= (pInfo->osd_rect[i].y_start +pInfo->osd_rect[i].height)))
			{
				return 1;
			}
		}
	}

	return 0;
}

/*
int IsRectIntersecting(int left_1, int top_1, int right_1, int bottom_1,
					   int left_2, int top_2, int right_2, int bottom_2)
{
	// 检查矩形合法性
	if(left_1 >= right_1 || top_1 >= bottom_1)
		return 0;

	if(left_2 >= right_2 || top_2 >= bottom_2)
		return 0;

	// 判断矩形是否相交
	return ((left_1 <= left_2 && right_1 <= left_2)
			|| (left_1 >= right_2 && right_1 >= right_2)
			|| (top_1 <= top_2 && bottom_1 <= top_2)
			|| (top_1 >= bottom_2 && bottom_1 >= bottom_2))? 0 : 1;
}
*/

int RgbYuvCmp(MonitorColorFieldThrld &stThrld, unsigned char byRY, unsigned char byGU, unsigned char byBV)
{
    //printf("R:%d RDown:%d RUP:%d   G:%d GDown:%d GUP:%d   B:%d BDown:%d BUP:%d\n", 
    //    byRY, stThrld.nRedYDown, stThrld.nRedYUP, byGU, stThrld.nGreenUDown, stThrld.nGreenUUp, byBV, stThrld.nBlueVDown, stThrld.nBlueVUp);

	if ((byRY >= stThrld.nRedYDown)&&(byRY <= stThrld.nRedYUP)
		&&(byGU >= stThrld.nGreenUDown)&&(byGU <= stThrld.nGreenUUp)
		&&(byBV >= stThrld.nBlueVDown)&&(byBV<= stThrld.nBlueVUp))
	{
		//符合门限值
		return 0;
	}
	return -1;
}

int alg_blackfreme_checkColorFeild(ALG_BLACKFRAME_InArgs *inArgs, MonitorColorFieldThrld &stThrldRed, 
    MonitorColorFieldThrld &stThrldGreen, MonitorColorFieldThrld &stThrldBlue, MonitorColorFieldThrld &stThrldBlack, 
    MonitorColorFieldThrld &stThrldGray, MonitorColorFieldThrld &stThrldWhite, unsigned short *uStatue)
{
	int w,h;
	int nY = 0, nU = 0, nV = 0; 

	int yoffset = 0;
	int findFlag = 0;


	//取一个OSD区域之外的16*16宏块
	while((inArgs->usYStart + 16 + yoffset < inArgs->nVideoHeight) && !findFlag)
	{
		findFlag = 1;
		
		for(h= inArgs->usYStart + yoffset;h< inArgs->usYStart + 16 + yoffset && findFlag;h++)
		{
			for ( w = inArgs->usXStart; w < inArgs->usXStart + 16 && findFlag; w++)
			{
				if(checkBlackFrameInOsd(h,w,inArgs))
				{
					yoffset += 16;
					findFlag = 0;
					continue;
				}
			}
		}
		
	}

	if(!findFlag)
		return 0;
	//计算16*16宏块的Y均值
	for (h = inArgs->usYStart + yoffset; h < inArgs->usYStart + yoffset + 16; h++)
	{
		for ( w = inArgs->usXStart; w < inArgs->usXStart + 16; w++)
		{
			nY += *(inArgs->cur_frame+ w + h * inArgs->nVideoWidth);
		}
	}
	nY /= 256;

	//计算8*8宏块的UV均值
	for (h = (inArgs->usYStart + yoffset)/2; h < (inArgs->usYStart + yoffset)/2 + 8; h++)
	{
		for ( w = inArgs->usXStart; w < inArgs->usXStart + 16; w+=2)
		{
			nV += *(inArgs->cur_CbCr_frame + w + h * inArgs->nVideoWidth);
			nU += *(inArgs->cur_CbCr_frame + w + 1 + h * inArgs->nVideoWidth);
		}
	}
	nU /= 64;
	nV /= 64;

//	printf("Corlor Y %d U %d V %d, yoffset = %d\n", nY, nU, nV, yoffset);

    // 清空彩场标志 下面判断是具体那种彩场
	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_COLORFRAME);

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_BLACK);
	if (0 == RgbYuvCmp(stThrldBlack, nY, nU, nV))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_BLACK);
		return 0;
	}

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_WHITE);
	if (0 == RgbYuvCmp(stThrldWhite, nY, nU, nV))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_WHITE);
		return 0;
	}

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_GRAY);
	if (0 == RgbYuvCmp(stThrldGray, nY, nU, nV))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_GRAY);
		return 0;
	}

	int nR = nY + 1.4075 * (nV - 128);
	int nG = nY - 0.3455 * (nU - 128) - 0.7169 * (nV - 128);
	int nB = nY + 1.779 * (nU - 128);

	//干掉负数
	nR = (nR < 0)?0:nR;
	nG = (nG < 0)?0:nG;
	nB = (nB < 0)?0:nB;

    //将超界数据拉回来
    nR = (nR > 255)?255:nR;
    nG = (nG > 255)?255:nG;
    nB = (nB > 255)?255:nB;

    //printf("11111 nY:%d nU:%d nV:%d nR:%d nG:%d nB:%d\n", nY, nU, nV, nR, nG, nB);

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_RED);
	if (0 == RgbYuvCmp(stThrldRed, nR, nG, nB))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_RED);
		return 0;
	}

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_GREEN);
	if (0 == RgbYuvCmp(stThrldGreen, nR, nG, nB))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_GREEN);
		return 0;
	}

	*uStatue = ClEAR_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_BLUE);
	if (0 == RgbYuvCmp(stThrldBlue, nR, nG, nB))
	{
		*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_CLORLFRAME_BLUE);
		return 0;
	}

    // 设置彩场标志 因为上面流程没有具体判断出是那种颜色的彩场 使用默认彩场标志位
	*uStatue = SET_BIT(*uStatue, WARN_TYPE_V_COLORFRAME);
	return 0;
}

void alg_blackfreme_videocheck
(
	ALG_BLACKFRAME_InArgs *pIn,
	ALG_BLACKFRAME_OutArgs *pOut,  
	unsigned char *pVideoImage,
	unsigned char *pStillImageRef,
	unsigned char *pTestImageRef[]
)
{
	int nVideoRow = 0, nVideoCol = 0;			// 当前视频帧行、列变量
	int nRefRow = 0, nRefCol = 0;				// 参考图像帧行、列变量
	int nVideoBlockX = 0, nVideoBlockY = 0;		// 视频图像帧像素块行、列变量
	int nRefBlockX = 0, nRefBlockY = 0;			// 参考图像帧像素块行、列变量

	int nVideoImagePos = 0, nImageRefPos = 0;	// 当前视频帧、参考图像帧像素位置
	int nVideoImagePos_Cur = 0, nImageRefPos_Cur = 0;
												// 当前图像帧、参考图像帧当前像素位置

	int nValidStillRef = 0;						// 静帧参考图像是否有效(初始化)
	int nLuminDiff = 0, nLuminDiffSum = 0, nAbsLuminDiffSum = 0;
												// 帧间像素亮度差累加和与亮度差绝对值及累加和
	int nLuminSum = 0, nPreLuminSum = 0;		// 图像像素亮度累加和及先前亮度累加和

	int nBlackFlag = 0, nStillFlag = 0;			// 黑场、静帧图像标志
	int nStillCount = 0;						// 标记静帧宏块的数目
	int nThredCount = 0;						// 非静帧宏块门限，若超过宏块总是得10%则认为非静帧，否则认为静帧
	int step = pIn->step;
	//nThredCount = 50;							//静帧宏块放大到50个
	int i =0;
	int ref_pos = 0;
	if(step < 1)
		step = 1;
	if(pVideoImage && pStillImageRef)
		nValidStillRef = 1;
	else
		goto end;

	// 根据故障检测掩码位，决定是否检测相应故障
	nBlackFlag = (pIn->bBlackEnable == 0)? 0 : 1;
	nStillFlag = (pIn->bStillEnable == 0)? 0 : 1;

	if(pIn->osd_count > 0)
	{
		for(i = pIn->usYStart; i < pIn->usYEnd;i++)
		{
			int ret  = 0;
			ret = checkBlackFrameInOsd(pIn->usXStart,i,pIn);
			if(ret == 0)
			{
				break;
			}
		}
		ref_pos = pIn->nVideoWidth*i+pIn->usXStart;
	}

	//超过10% 认为非静帧
	nThredCount = ((pIn->usYEnd -  pIn->usYStart)/(16*step)) * ((pIn->usXEnd -  pIn->usXStart)/(16*step)) * pIn->stillRatio; // 10%
	
	// 以16*16像素块为单位进行分析
	// 注意：如果实际检测区域(去除边缘和测试图动态区域后)不包含任何块，则会导致误报黑场、测试图、静帧等故障。
	//		 由于这种情况往往是由于设置不合理造成，一般应用情况下不会出现，所以在这里考虑到效率问题，不再检查。
	for(nVideoRow = pIn->usYStart, nRefRow = 0;
			nVideoRow + 16 <= pIn->usYEnd; nVideoRow += 16*step, nRefRow += 16*step)
	{
		for(nVideoCol = pIn->usXStart, nRefCol = 0;
				nVideoCol + 16 <= pIn->usXEnd; nVideoCol += 16*step, nRefCol += 16*step)
		{		
			nLuminSum = 0;
			nLuminDiffSum = 0;
			nAbsLuminDiffSum = 0;

			// 计算当前块左上角像素位置
			nVideoImagePos = nVideoRow * pIn->nVideoWidth + nVideoCol;
			nImageRefPos = nRefRow * pIn->nImageRefWidth + nRefCol;

			// 以16*16 小像素块为单位，仅取左上角代表像素，同时进行静帧、黑场分析
			for(nVideoBlockY = 0, nRefBlockY = 0; nVideoBlockY < 16; nVideoBlockY += 1, nRefBlockY++)
			{
				for(nVideoBlockX = 0, nRefBlockX = 0; nVideoBlockX < 16; nVideoBlockX += 1, nRefBlockX++)
				{
					int ret = 0;
					nVideoImagePos_Cur = nVideoImagePos + nVideoBlockY * pIn->nVideoWidth + nVideoBlockX;
					nImageRefPos_Cur = nImageRefPos + nRefBlockY * pIn->nImageRefWidth + nRefBlockX;
					
					ret = checkBlackFrameInOsd(nVideoCol+nVideoBlockX,nVideoRow+nVideoBlockY,pIn);
					if(ret == 1)
					{
						nVideoImagePos_Cur = ref_pos;
						nImageRefPos_Cur = ref_pos;
					}
					
					// 黑场检测计算
					if(nBlackFlag)
						nLuminSum += (unsigned char)pVideoImage[nVideoImagePos_Cur];

					// 静帧检测计算
					if(nStillFlag && nValidStillRef)
					{
						//nLuminDiff = (int)pVideoImage[nVideoImagePos_Cur] - (int)pStillImageRef[nImageRefPos_Cur];
						nLuminDiff = (int)pVideoImage[nVideoImagePos_Cur] - (int)pStillImageRef[nVideoImagePos_Cur];
						
						nLuminDiffSum += nLuminDiff;
						nAbsLuminDiffSum += abs(nLuminDiff);
					}

					// 参考帧由外面传进来，所以此处不赋值
					// 将当前图像像素值作为静帧参考
					//pStillImageRef[nImageRefPos_Cur] = pVideoImage[nVideoImagePos_Cur];
				}
			}
			
			// 在帧内，将16*16像素块的亮度累加和与前一像素块作差，判断是否为黑场
			if(nBlackFlag
				&& !(nRefRow == 0 && nRefCol == 0)// 从第2个块开始处理
				&& abs(nLuminSum - nPreLuminSum) > pIn->usBlack)// 亮度变化超出范围，不是黑场
			{	
				//pOut->outargs.extendedError = abs(nLuminSum - nPreLuminSum); // for test
				nBlackFlag = 0;				
			}
			else
			{
				nPreLuminSum = nLuminSum;
			}

			// 以16*16像素块为单位，比较帧间像素块的亮度变化，判断是否为静帧
			nLuminDiffSum = abs(nLuminDiffSum);
			if(nStillFlag && nValidStillRef
				&& ((nAbsLuminDiffSum > (10*pIn->usStill)) || (nLuminDiffSum > pIn->usStill && nAbsLuminDiffSum > (2*pIn->usStill))))
			{
				pOut->outargs.extendedError = nAbsLuminDiffSum;
				pOut->outargs.currentAU = nLuminDiffSum;
				nStillCount++;
				if(nStillCount > nThredCount)
				{
					nStillFlag = 0;
				}
			}

			//add by huanglb
			if ((0 == nStillFlag) && (0 == nBlackFlag))
			{
				//返回
				goto end;
			}
		}
	}
	
end:
		
	pOut->bBlackFlag = nBlackFlag;
	pOut->bStillFlag = nStillFlag;
	pOut->usStillCnt = nStillCount;
	pOut->bTestImgFlag[0] = nStillCount;
}

/*
 *  ======== IMGENC1COPY_TI_process ========
 */
int alg_blackframe_process(ALG_BLACKFRAME_InArgs *inArgs, ALG_BLACKFRAME_OutArgs *outArgs)
{
	int numInBytes = 0;
    //Uint32       thisTransferSrcAddr, thisTransferDstAddr;

	ALG_BLACKFRAME_InArgs *pCheckInArgs = inArgs;
	ALG_BLACKFRAME_OutArgs *pCheckOutArgs = outArgs;

	unsigned char *pTestImgBufs[MAX_TESTIMAGE_NUM];

   
	numInBytes = inArgs->nVideoWidth * inArgs->nVideoHeight;

	inArgs->nImageRefWidth = (pCheckInArgs->usXEnd - pCheckInArgs->usXStart);
	inArgs->nImageRefHeight = (pCheckInArgs->usYEnd - pCheckInArgs->usYStart);


	pCheckOutArgs->outargs.extendedError = 0;
	pCheckOutArgs->outargs.currentAU = 0;

	// 黑场、静帧、侧视图监测
	alg_blackfreme_videocheck(pCheckInArgs, pCheckOutArgs, 
		(unsigned char *)(inArgs->cur_frame), (unsigned char *)(inArgs->last_frame), pTestImgBufs);

    /* outArgs->bytesGenerated reports the total number of bytes encoded */
    pCheckOutArgs->outargs.bytesGenerated = numInBytes;

    return 0;
}


int alg_blackframe_printf_info(char *print_buf,int iCh,ALG_BLACKFRAME_InArgs input,ALG_BLACKFRAME_OutArgs output)
{
	char *p = NULL;
	p = print_buf;

	p += sprintf(p,"%-3d|%-11u|%-11u|%-11f|%-5u|%-5u|%-6u|%-5u|%-6u|%-5u|%-9d|%-9d|%-9d|%-14d|%-9d|%-14d\r\n"
					,iCh
					,input.bBlackEnable
					,input.bStillEnable
					,input.stillRatio
					,input.usBlack
					,input.usStill
					,input.usXStart
					,input.usXEnd
					,input.usYStart
					,input.usYEnd
					,output.bBlackFlag
					,output.bStillFlag
					,output.usStillCnt
					,output.outargs.bytesGenerated
					,output.outargs.currentAU
					,output.outargs.extendedError);
	/*p += sprintf(p,FORMAT_TITLE,"alg blackframe");
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input bBlackEnable",input.bBlackEnable);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input bStillEnable",input.bStillEnable);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usBlack",input.usBlack);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usStill",input.usStill);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usXStart",input.usXStart);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usXEnd",input.usXEnd);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usYStart",input.usYStart);
	p += sprintf(p,FORMAT_32STR_UINT,"alg bf input usYEnd",input.usYEnd);
	
	p += sprintf(p,FORMAT_32STR_INT,"alg bf output bBlackFlag",output.bBlackFlag);
	p += sprintf(p,FORMAT_32STR_INT,"alg bf output bStillFlag",output.bStillFlag);
	p += sprintf(p,FORMAT_32STR_INT,"alg bf output Generated",output.outargs.bytesGenerated);
	p += sprintf(p,FORMAT_32STR_INT,"alg bf output currentAU",output.outargs.currentAU);
	p += sprintf(p,FORMAT_32STR_INT,"alg bf output extenderr",output.outargs.extendedError);*/
	
	return (p-print_buf);
}

