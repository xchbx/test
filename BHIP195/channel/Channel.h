#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "Common.h"
#include "ChannelStruct.h"
#include "SysConfig.h"
#include "ZThread.h"
#include "StreamReader.h"
#include "StreamWriter.h"
#include "VideoES2YUV.h"
#include "VideoYUV2ES.h"
#include "AudioES2PCM.h"
#include "AudioPCM2ES.h"
#include "Monitor.h"
#include "OsdMonitor.h"


struct AVSFrame;


// 通道模
class Channel
{
public:
    explicit Channel(SysChannel &stSysChannel);
    ~Channel(void);

    bool Init();        // 读取配置文件参数

    bool Start();
    void Stop();

    bool GetRunStatus() { return m_bRun; }
    bool GetWarnStatus(unsigned short &sWarnVedioStatus, unsigned short &sWarnAudioStatus, string &strOutIP, int &nOutPort);

    static void S_WriteYUVData(void *pSurface, unsigned long long ullPTS, int nFrameRate, void *pContext);
    static void S_WritePCMData(unsigned char *pPcmData, unsigned int uLen, unsigned long long ullPTS, unsigned int uSamplesNumber, void *pContext);

public:
    // 设置/获取通道转码参数
    void SetTransParam(TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam);
    void GetTransParam(TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam);

    // 设置/获取通道OSD参数
    void SetOSDParam(OsdInfo &stOSD);
    void GetOSDParam(OsdInfo &stOSD) { stOSD = m_stOSD; }

    // 设置/获取通道输出组播地址及开关
    void SetMultiAddr(TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr);
    void GetMultiAddr(TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr) { stTSOutAddr = m_stTSOutAddr; stMonitorOutAddr = m_stMonitorOutAddr; }


private:
    // 工作线程函数
    static Tool::TH_RET TH_Work(LPVOID lpContext);
    int Work();

    bool VideoDataProcess();    // 视频处理(解码缩放YUV)
    bool AudioDataProcess();    // 音频处理监测功能

    // 视频处理(编码)监测功能
    void VideoDataProcess2(mfxFrameSurface1 *pSurface, unsigned long long ullPTS);
    void VideoDataProcess2(AVSFrame *pAVSFrame, unsigned long long ullPTS, int nFrameRate);

    void VideoDataProcess2(unsigned long long ullPTS, int nFrameRate);

    // 音频处理(编码)监测功能
    void AudioDataProcess2(unsigned char *pPcmData, unsigned int uPcmDataLen, unsigned long long ullPTS, unsigned int uSamplesNumber);

    // 通道的监测故障状态 记录
    bool MonitorStatusRecord(unsigned long long ullCurTime);

    // 黑场静帧内存数据是否已经申请完毕
    bool HaveBlackFrameData();

private:
    // 为参数实现改变提供ReStart接口
    void CheckReStartFlag();

    bool ReStartTSOut();
    //bool ReStartMonitorOut();     // 实时

    bool ReStartTSIn();
    bool ReStartVideoES2YUV();
    bool ReStartVideoYUV2ES();

    bool ReStartAudioES2PCM();
    bool ReStartAudioPCM2ES();

    //bool ReStartOSD();              // 实时


private:
    bool                m_bRun;
    Tool::TH_HANDLE     m_hWorkThread;

    CStreamReader     * m_pStreamReader;
    CStreamWriter     * m_pStreamWriter;

    // 视频
    VideoES2YUV       * m_pVideoES2YUV;
    VideoYUV2ES       * m_pVideoYUV2ES;

    // 音频
    AudioES2PCM       * m_pAudioES2PCM;
    AudioPCM2ES       * m_pAudioPCM2ES;


    // 监测相关
    Monitor             m_objMonitor;

    unsigned long long  m_ullLastRecvVideoTime;
    unsigned long long  m_ullLastRecvAudioTime;

    MonitorWarnStatus   m_stMonitorWarnStatus;  // 监测告警状态
    unsigned short      m_sLastWarnVideoStatus; // 上次视频告警状态
    unsigned short      m_sLastWarnAudioStatus; // 上次音频告警状态


    unsigned long long  m_ullChannelStartTime;  // 用于在Channel开始码流未稳定时不进行监测 3S后开始监测


    // 断流输出黑场帧相关
    unsigned char     * m_pBlackFrameYAddr;    // 黑场帧Y数据
    int                 m_nBlackFrameYSize;
    unsigned char     * m_pBlackFrameUVAddr;   // 黑场帧UV数据
    int                 m_nBlackFrameUVSize;
    
    // 断流输出PTS相关
    unsigned long long  m_ullLastVedioPTS;              // 最近一次视频PTS值
    unsigned long long  m_ullLastSendBlackFrameTime;   // 上次发送黑场静帧的时间(断流导致)


private:
    int                 m_nIndex;
    
    TSInParam           m_stTSInParamProtocol;  // 协议中参数信息
    TSOutParam          m_stTSOutParamProtocol;

    TSInParam           m_stTSInParam;          // 正在使用的参数信息
    TSOutParam          m_stTSOutParam;

    TSInAddr            m_stTSInAddr;
    TSOutAddr           m_stTSOutAddr;
    MonitorOutAddr      m_stMonitorOutAddr;
    OsdInfo             m_stOSD;

	//COsdMonitor         m_objOsdMonitor;
    COsdMonitor       * m_pOsdMonitor;

	OS_Mutex            m_mutexParamReset;
	char			    m_cParamReset;
};

#endif

