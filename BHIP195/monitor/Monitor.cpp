#include "Monitor.h"
#include "OSTime.h"


Monitor::Monitor()
: m_ullLastMonitorTime(0)
, m_ullStillHappendTime(0)
{

}

Monitor::~Monitor()
{

}

int Monitor::ColorbarSetParam(unsigned char *pYaddr, int width, int height)
{
    //m_stColorBarInArgs.pbyData = g_capture_link[iCh].cur_frameYaddr;
    m_stColorBarInArgs.pbyData = pYaddr;
    m_stColorBarInArgs.puAverage = m_arrAverage;

    m_stColorBarInArgs.uCutLeftWidth = m_stThrldColorBar.nXStart * width/352;			//视频采样区域X轴方向起始点，默认30
    m_stColorBarInArgs.uCutRightWidth = (352 - m_stThrldColorBar.nXEnd) * width/352;	//视频采样区域X轴方向结束点，默认322
    m_stColorBarInArgs.uCutLowHeight = (288 - m_stThrldColorBar.nYEnd) * height/288;	//视频采样区域Y轴方向结束点，默认258
    m_stColorBarInArgs.uCutUpHeight = m_stThrldColorBar.nYStart * height/288;			//视频采样区域Y轴方向起始点，默认30

    m_stColorBarInArgs.uMaxThredValue = m_stThrldColorBar.nMaxDiffValue;
    m_stColorBarInArgs.uMinCorlorBarNum = m_stThrldColorBar.nMinNum;

    m_stColorBarInArgs.uWidth = width;
    m_stColorBarInArgs.uHeight = height;

    if (width - m_stColorBarInArgs.uCutRightWidth - m_stColorBarInArgs.uCutLeftWidth < 100)
    {
        //帧宽度不能小于100
        m_stColorBarInArgs.uCutLeftWidth = 30;
        m_stColorBarInArgs.uCutRightWidth = 30;
    }

    if(height -m_stColorBarInArgs.uCutLowHeight - m_stColorBarInArgs.uCutUpHeight < 100)
    {
        //帧高度不能小于100
        m_stColorBarInArgs.uCutLowHeight = 30;
        m_stColorBarInArgs.uCutUpHeight = 30;
    }

    m_stColorBarInArgs.osd_count = 0;
    //for(int i = 0; i < g_capture_MainParam.osd_count[iCh]; i++)
    //    osd_thr_get_rect_info(iCh,i,&g_capture_link[iCh].AlgColorBar_input.osd_rect[i].x_start
    //    ,&g_capture_link[iCh].AlgColorBar_input.osd_rect[i].y_start
    //    ,&g_capture_link[iCh].AlgColorBar_input.osd_rect[i].width
    //    ,&g_capture_link[iCh].AlgColorBar_input.osd_rect[i].height
    //    );


#if 0
    printf("colorbar left = %d, right = %d, up = %d, low = %d; videoW = %d, videoH = %d\r\n"
        ,m_stColorBarInArgs.uCutLeftWidth
        ,m_stColorBarInArgs.uCutRightWidth
        ,m_stColorBarInArgs.uCutUpHeight
        ,m_stColorBarInArgs.uCutLowHeight
        ,m_stColorBarInArgs.uWidth
        ,m_stColorBarInArgs.uHeight
        );
#endif

    return 0;
}

int Monitor::BlackFrameSetParam(unsigned char *pYaddr, unsigned char *pUVaddr, int width, int height, int ChromaFomart)
{
    int i = 0;
    float fX, fY;

    fX = (float)width / 352.0;
    fY = (float)height / 288.0;

    //m_stBlackframeInArgs.nCh = iCh;
    m_stBlackframeInArgs.nVideoWidth = width;
    m_stBlackframeInArgs.nVideoHeight = height;
    m_stBlackframeInArgs.nVideoFormat = ChromaFomart;

    m_stBlackframeInArgs.bBlackEnable = m_stMonitorEnable.nVideoMonitor & 0x01;
    m_stBlackframeInArgs.bStillEnable = (m_stMonitorEnable.nVideoMonitor >> 1)& 0x01;

    m_stBlackframeInArgs.usStill	= m_stThrldVideo.nStill;
    m_stBlackframeInArgs.stillRatio = m_stThrldVideo.nStillRatio/1000.0;
    m_stBlackframeInArgs.usBlack = m_stThrldVideo.nBlack;
    m_stBlackframeInArgs.usXStart = (unsigned short)(m_stThrldVideo.nXStart * fX);
    m_stBlackframeInArgs.usYStart = (unsigned short)(m_stThrldVideo.nYStart * fY);
    m_stBlackframeInArgs.usXEnd = (unsigned short)(m_stThrldVideo.nXEnd * fX);
    m_stBlackframeInArgs.usYEnd = (unsigned short)(m_stThrldVideo.nYEnd * fY);

    m_stBlackframeInArgs.usXStart = (m_stBlackframeInArgs.usXStart + 1)/2 *2;
    m_stBlackframeInArgs.usYStart = (m_stBlackframeInArgs.usYStart + 1)/2 *2;
    m_stBlackframeInArgs.usXEnd = (m_stBlackframeInArgs.usXEnd + 1)/2 *2;
    m_stBlackframeInArgs.usYEnd = (m_stBlackframeInArgs.usYEnd + 1)/2 *2;

    if(width <= 352)
    {
        m_stBlackframeInArgs.step = 1; //for best result
    }
    else
        m_stBlackframeInArgs.step = 2;


    m_stBlackframeInArgs.cur_frame = pYaddr;            // YAddr
    m_stBlackframeInArgs.cur_CbCr_frame = pUVaddr;      // UVAddr
    m_stBlackframeInArgs.last_frame = m_arrLastFrameYaddr;

    m_stBlackframeInArgs.osd_count = 0;
    //for(i = 0; i < g_capture_MainParam.osd_count[iCh];i++)
    //    osd_thr_get_rect_info(iCh,i,&m_stBlackframeInArgs.osd_rect[i].x_start
    //    ,&m_stBlackframeInArgs.osd_rect[i].y_start	
    //    ,&m_stBlackframeInArgs.osd_rect[i].width
    //    ,&m_stBlackframeInArgs.osd_rect[i].height
    //    );

#if 0
    printf("black frame xstart = %d, yStart = %d, xEnd = %d, yEnd = %d; videoW = %d, videoH = %d\r\n"
        ,m_stBlackframeInArgs.usXStart
        ,m_stBlackframeInArgs.usYStart
        ,m_stBlackframeInArgs.usXEnd
        ,m_stBlackframeInArgs.usYEnd
        ,m_stBlackframeInArgs.nVideoWidth
        ,m_stBlackframeInArgs.nVideoHeight
        );
#endif

    return 0;
}

int Monitor::VideoDataMonitor(unsigned char *pYaddr, unsigned char *pUVaddr, int width, int height, unsigned short &sMonitorWarnStatus)
{
    unsigned long long ullCurTime = OS_SysTime_Ms();
    if (ullCurTime - m_ullLastMonitorTime < m_stThrldVideo.nSampleTime)
    {
        return 0;
    }
    m_ullLastMonitorTime = ullCurTime;

    // colorbar alg 彩条
    ColorbarSetParam(pYaddr, width, height);
    alg_colorbar_process(&m_stColorBarInArgs, &m_stColorBarOutArgs);

    if (0 == m_stColorBarOutArgs.colorBarCheck)
    {
        sMonitorWarnStatus |= (1 << WARN_TYPE_V_COLORBAR);
    }
	else
	{
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_COLORBAR);	
	}

    int ChromaFomart = 0;
    BlackFrameSetParam(pYaddr, pUVaddr, width, height, ChromaFomart);
    alg_blackframe_process(&m_stBlackframeInArgs, &m_stBlackframeOutArgs);

    // blackframe alg 色场
    if (1 == m_stBlackframeOutArgs.bBlackFlag)
    {
        //判断是哪种颜色的色场
        alg_blackfreme_checkColorFeild(&m_stBlackframeInArgs, m_stThrldRed, m_stThrldGreen, m_stThrldBlue, 
                                        m_stThrldBlack, m_stThrldGray, m_stThrldWhite, &sMonitorWarnStatus);
    }
	else if (0 == m_stBlackframeOutArgs.bBlackFlag)
	{
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_BLACK);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_CLORLFRAME_WHITE);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_CLORLFRAME_GRAY);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_CLORLFRAME_RED);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_CLORLFRAME_GREEN);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_CLORLFRAME_BLUE);
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_COLORFRAME);
	}

    //静帧
    if (1 == m_stBlackframeOutArgs.bStillFlag)
    {
        sMonitorWarnStatus |= (1 << WARN_TYPE_V_STILL);
        m_ullStillHappendTime = ullCurTime;
    }
    else
    {
		sMonitorWarnStatus &= ~(1 << WARN_TYPE_V_STILL);

        if (ullCurTime < m_ullStillHappendTime)	//时间翻转
        {
            m_ullStillHappendTime = ullCurTime;
        }		
        else if (ullCurTime - m_ullStillHappendTime >= m_stThrldVideo.nStill)
        {
            memcpy(m_arrLastFrameYaddr, pYaddr, width*height);     // 将上一帧数据Y数据备份
        }
    }

    return 0;
}

int Monitor::AudioDataMonitor(unsigned char *pPCMData, int nChannels, int nPcmSamplesPerFrame, int nBitPerSample, unsigned short &sMonitorWarnStatus)
{
    //init g_snd_audio_set_infos

    alg_snd_process(m_stThrldAudio, m_stAudioInfos, m_stAudioPrintInfo, m_arrCalcCnt, m_arrWarnCnt, (signed short *)pPCMData, nChannels, nPcmSamplesPerFrame, nBitPerSample, &sMonitorWarnStatus);

    return 0;
}

