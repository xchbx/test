#pragma once

#include "ChannelStruct.h"

#include "alg_colorbar.h"
#include "alg_blackframe.h"
#include "alg_snd.h"


#define MAX_WIDTH_PIXEL_NUM 4096


class Monitor
{
public:
    Monitor();
    ~Monitor();

    int VideoDataMonitor(unsigned char *pYaddr, unsigned char *pUVaddr, int width, int height, unsigned short &sMonitorWarnStatus);
    int AudioDataMonitor(unsigned char *pPCMData, int nChannels, int nPcmSamplesPerFrame, int nBitPerSample, unsigned short &sMonitorWarnStatus);

private:
    int BlackFrameSetParam(unsigned char *pYaddr, unsigned char *pUVaddr, int width, int height, int ChromaFomart);
    int ColorbarSetParam(unsigned char *pYaddr, int width, int height);


public:
    // 监测参数相关 协议中暂未设置 使用配置文件中的默认参数
    MonitorEnable               m_stMonitorEnable;
    MonitorVThrld               m_stThrldVideo;
    MonitorAThrld               m_stThrldAudio;
    MonitorColorBarThrld        m_stThrldColorBar;      // 彩条
    MonitorColorFieldThrld      m_stThrldRed;           // 红色彩场
    MonitorColorFieldThrld      m_stThrldGreen;         // 绿色彩场
    MonitorColorFieldThrld      m_stThrldBlue;          // 蓝色彩场
    MonitorColorFieldThrld      m_stThrldBlack;         // 黑色彩场
    MonitorColorFieldThrld      m_stThrldGray;          // 灰色彩场
    MonitorColorFieldThrld      m_stThrldWhite;         // 白色彩场


    // 视频
private:
    // 彩条监测相关
    unsigned int			m_arrAverage[MAX_WIDTH_PIXEL_NUM];    // 存储像素均值
    ALG_COLORBAR_InArgs     m_stColorBarInArgs;
    ALG_COLORBAR_OutArgs    m_stColorBarOutArgs;


    // 彩场监测相关
    ALG_BLACKFRAME_InArgs	m_stBlackframeInArgs;
    ALG_BLACKFRAME_OutArgs	m_stBlackframeOutArgs;
    unsigned char		    m_arrLastFrameYaddr[MAX_DISPLAY_WIDTH_HD * MAX_DISPLAY_HEIGHT_HD];


    unsigned long long      m_ullLastMonitorTime;       // 上一次监测时间
    // 监测告警结果
    unsigned long long      m_ullStillHappendTime;      // 静帧发生时间


private:
    // 音频
    ALG_AUDIO_Infos         m_stAudioInfos;
    ALG_AUDIO_Infos  		m_stAudioPrintInfo;

    int                     m_arrCalcCnt[2][3];		// 采样周期内的采样次数
    int                     m_arrWarnCnt[2][3];     // 采样周期内的故障次数
};

