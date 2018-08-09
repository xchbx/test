#ifndef __ALG_COLORBAR_H__
#define __ALG_COLORBAR_H__

#include "Common.h"


struct ALG_COLORBAR_InArgs{
	//用户设置
	unsigned int uMaxThredValue;	// 最大像素差值阈值 默认150, 模拟彩条均值80-100 < 150 
	unsigned int uMinCorlorBarNum;	// 最小彩条数 默认3
	unsigned int uHeight;			// 帧的高度MAX_WIDTH
	unsigned int uWidth;			// 帧的宽度MAX_HEIGHT
	unsigned int uCutLeftWidth;		// 裁剪区域
	unsigned int uCutRightWidth;    // 距离右边的距离
	unsigned int uCutUpHeight;
	unsigned int uCutLowHeight;     // 距离下边的距离

	unsigned char *pbyData;		    // Y值地址
	unsigned int *puAverage;	    // 均值的存储地址
	unsigned int osd_count;
	OSD_RECT_INFO osd_rect[MAX_OSD_NUM];

    ALG_COLORBAR_InArgs(): uMaxThredValue(0), uMinCorlorBarNum(0), uHeight(0), uWidth(0),
        uCutLeftWidth(0), uCutRightWidth(0), uCutUpHeight(0), uCutLowHeight(0), pbyData(NULL),
        puAverage(NULL), osd_count(0)
    {
        
    }
};

struct ALG_COLORBAR_OutArgs{
	int colorBarCheck;	/*The result of colorbar check*/
	int colorAverage;	/*The result of colorbar Average*/

    ALG_COLORBAR_OutArgs(): colorAverage(0), colorBarCheck(0)
    {

    }
};


int alg_colorbar_process(ALG_COLORBAR_InArgs *pAlgcolorbar_input, ALG_COLORBAR_OutArgs *pAlgcolorbar_output);
int alg_colorbar_printf_info(char *print_buf, int iCh, ALG_COLORBAR_InArgs input, ALG_COLORBAR_OutArgs output);

#endif

