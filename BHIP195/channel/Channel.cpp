#include "Channel.h"
#include "DraES2PCM.h"
#include "FFMPEGES2PCM.h"
#include "IntelES2YUV.h"
#include "AVSES2YUV.h"

const unsigned long long g_ullMonitorReportInterval = 500;  // 监测告警500ms上报一次

// 视频码流中断PTS相关
static const int g_nBlackFrameRate = 25;			// 断流输出黑场帧率
static const int g_nBlackSendInterval = 40;         // 断流后每40ms发送一帧黑场帧
static const int g_nBlackPTSIncrement = g_nBlackSendInterval * 90;     // 每次发送黑场帧PTS增量


enum RESTART_FLAG
{
    RESTART_TSOUT = 0,
    RESTART_TSIN,
    RESTART_VIDEO_ES2YUV,
    RESTART_VIDEO_YUV2ES,
    RESTART_AUDIO_ES2PCM,
    RESTART_AUDIO_PCM2ES,

    RESTART_MAX_INDEX
};


Channel::Channel(SysChannel &stSysChannel)
: m_bRun(false)
, m_hWorkThread(Tool::CZThread::INVALID_HANDLE)
, m_pStreamReader(NULL)
, m_pStreamWriter(NULL)
, m_pVideoES2YUV(NULL)
, m_pVideoYUV2ES(NULL)
, m_pAudioES2PCM(NULL)
, m_pAudioPCM2ES(NULL)
, m_ullLastRecvVideoTime(0)
, m_ullLastRecvAudioTime(0)
, m_sLastWarnVideoStatus(0)
, m_sLastWarnAudioStatus(0)
, m_pBlackFrameYAddr(NULL)
, m_nBlackFrameYSize(0)
, m_pBlackFrameUVAddr(NULL)
, m_nBlackFrameUVSize(0)
, m_ullChannelStartTime(0)
, m_ullLastVedioPTS(0)
, m_ullLastSendBlackFrameTime(0)
, m_nIndex(stSysChannel.nIndex)
, m_pOsdMonitor(NULL)
{
    // 使用配置文件中参数进行赋值
    m_stTSInAddr = stSysChannel.stTSInAddr;
    m_stTSInParam = m_stTSInParamProtocol = stSysChannel.stTSInParam;
    m_stTSOutAddr = stSysChannel.stTSOutAddr;
    m_stTSOutParam = m_stTSOutParamProtocol = stSysChannel.stTSOutParam;
    m_stMonitorOutAddr = stSysChannel.stMonitorOutAddr;
    m_stOSD = stSysChannel.stOSD;

    m_objMonitor.m_stMonitorEnable = stSysChannel.stMonitorEnable;
    m_objMonitor.m_stThrldVideo = stSysChannel.stThrldVideo;
    m_objMonitor.m_stThrldAudio = stSysChannel.stThrldAudio;
    m_objMonitor.m_stThrldColorBar = stSysChannel.stThrldColorBar;
    m_objMonitor.m_stThrldRed = stSysChannel.stThrldRed;
    m_objMonitor.m_stThrldGreen = stSysChannel.stThrldGreen;
    m_objMonitor.m_stThrldBlue = stSysChannel.stThrldBlue;
    m_objMonitor.m_stThrldBlack = stSysChannel.stThrldBlack;
    m_objMonitor.m_stThrldGray = stSysChannel.stThrldGray;
    m_objMonitor.m_stThrldWhite = stSysChannel.stThrldWhite;

	m_cParamReset = 0;

	m_mutexParamReset.Init();

	SetOSDParam(m_stOSD);
}

Channel::~Channel(void)
{
    Stop();
}

bool Channel::Start()
{
    if(m_bRun) return false;

    // 以最新参数开启  不需要重启动作
    m_mutexParamReset.Lock();
    m_cParamReset = 0;
    m_stTSInParam = m_stTSInParamProtocol;
    m_stTSOutParam = m_stTSOutParamProtocol;
    m_mutexParamReset.Unlock();

    // StreamReader Create
    ReStartTSIn();

    // Video ES2YUV
    ReStartVideoES2YUV();

    // Video YUV2ES
    ReStartVideoYUV2ES();

    // Audio ES2PCM
    ReStartAudioES2PCM();

    // Audio PCM2ES
    ReStartAudioPCM2ES();

    // StreamWriter
    ReStartTSOut();

    m_bRun = true;
    m_ullChannelStartTime = OS_SysTime_Ms();

    m_hWorkThread = Tool::CZThread::BeginThread(TH_Work, this);
    if(Tool::CZThread::INVALID_HANDLE == m_hWorkThread)
    {
        printf("Channel::Start TH_Work Failed");
        m_bRun = false;
        return false;
    }

    return true;
}

void Channel::Stop()
{
    if(!m_bRun) return ;
    m_bRun = false;
    m_ullChannelStartTime = 0;

    Tool::CZThread::WaitThreadEnd(m_hWorkThread);

    SAFE_DELETE(m_pStreamReader);
    SAFE_DELETE(m_pStreamWriter);

    SAFE_DELETE(m_pVideoES2YUV);
    SAFE_DELETE(m_pVideoYUV2ES);

    SAFE_DELETE(m_pAudioES2PCM);
    SAFE_DELETE(m_pAudioPCM2ES);

    SAFE_FREE(m_pBlackFrameYAddr);
    SAFE_FREE(m_pBlackFrameUVAddr);
}

void Channel::SetTransParam(TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam)
{
    char cParamResetTmp = 0;

    if(m_stTSInAddr.nPort != stTSInAddr.nPort || m_stTSInAddr.strIP != stTSInAddr.strIP || 
       m_stTSInParam.nVPID != stTSInParam.nVPID)
    {
		cParamResetTmp |= (1 << RESTART_VIDEO_ES2YUV);
        cParamResetTmp |= (1 << RESTART_TSIN);
    }

    // 比较音频PID列表
    if((m_stTSInParam.vecAPID.empty() || stTSInParam.vecAPID.empty()) || (m_stTSInParam.vecAPID[0] != stTSInParam.vecAPID[0]))
    {
        cParamResetTmp |= (1 << RESTART_TSIN);
    }

	// 比较输入视频流类型
    if(m_stTSInParam.nVStreamType != stTSInParam.nVStreamType)
    {
        cParamResetTmp |= (1 << RESTART_VIDEO_ES2YUV);
        cParamResetTmp |= (1 << RESTART_VIDEO_YUV2ES);
    }

	//比较输出参数是否不同
    if(m_stTSOutParam.nVStreamType != stTSOutParam.nVStreamType || m_stTSOutParam.nWidth != stTSOutParam.nWidth ||
       m_stTSOutParam.nHeight != stTSOutParam.nHeight || m_stTSOutParam.nVRate != stTSOutParam.nVRate ||
       m_stTSOutParam.nFrameRate != stTSOutParam.nFrameRate)
    {
		cParamResetTmp |= (1 << RESTART_VIDEO_ES2YUV);
        cParamResetTmp |= (1 << RESTART_VIDEO_YUV2ES);
		cParamResetTmp |= (1 << RESTART_TSOUT);
    }

    if(m_stTSInParam.nAStreamType != stTSInParam.nAStreamType)
    {
        cParamResetTmp |= (1 << RESTART_AUDIO_ES2PCM);
    }

    if(m_stTSOutParam.nAStreamType != stTSOutParam.nAStreamType || m_stTSOutParam.nARate != stTSOutParam.nARate)
    {
        cParamResetTmp |= (1 << RESTART_AUDIO_PCM2ES);
    }

    if(0 != cParamResetTmp)
    {
        m_mutexParamReset.Lock();
        m_cParamReset |= cParamResetTmp; 
        m_mutexParamReset.Unlock();
    }

    // 转码输入参数设定
    m_stTSInAddr = stTSInAddr;
    m_stTSInParamProtocol = stTSInParam;        //m_stTSInParam = stTSInParam;

    // 转码输出参数设定 输出地址参数另外设定(非同一条协议)
    m_stTSOutParamProtocol = stTSOutParam;      //m_stTSOutParam = stTSOutParam;
}

void Channel::GetTransParam(TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam)
{
    // 转码输入参数设定
    stTSInAddr = m_stTSInAddr;
    stTSInParam = m_stTSInParamProtocol;

    // 转码输出参数设定 输出地址参数另外设定(非同一条协议)
    stTSOutParam = m_stTSOutParamProtocol;
}

void Channel::SetOSDParam(OsdInfo &stOSD)
{
    m_stOSD = stOSD;
#if 1
    if (m_pOsdMonitor)
    {
        m_pOsdMonitor->SetOsdInfo(stOSD);
    }
#else
    m_objOsdMonitor.SetOsdInfo(stOSD);
#endif
}

void Channel::SetMultiAddr(TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr)
{
    if(m_stTSOutAddr.nPort != stTSOutAddr.nPort || m_stTSOutAddr.strIP != stTSOutAddr.strIP)
    {
        m_mutexParamReset.Lock();
        m_cParamReset |= (1 << RESTART_TSOUT);
        m_mutexParamReset.Unlock();
    }

    m_stTSOutAddr = stTSOutAddr;
    m_stMonitorOutAddr = stMonitorOutAddr;
}

bool Channel::ReStartTSOut()
{
    // Stop
    SAFE_DELETE(m_pStreamWriter);

    // Start
    m_pStreamWriter = new CUdpStreamWriter();
    if (NULL == m_pStreamWriter)
    {
        printf("Channel::ReStartTSOut Failed\n");
        return false;
    }

    m_pStreamWriter->Init(m_stTSOutAddr, m_stTSOutParam);

    return true;
}

bool Channel::ReStartTSIn()
{
    log_info("restart channel %d", m_nIndex);
    // Stop
    SAFE_DELETE(m_pStreamReader);

    // Start
    m_pStreamReader = new CStreamReader(m_nIndex);
    if (NULL == m_pStreamReader)
    {
        return false;
    }
    m_pStreamReader->Start(m_stTSInAddr, m_stTSInParam);

    return true;
}

bool Channel::ReStartVideoES2YUV()
{
    log_info("restart channel %d", m_nIndex);
    // Stop
    SAFE_DELETE(m_pVideoES2YUV);

    // Start
    if (AV_CODEC_V_AVS == m_stTSInParam.nVStreamType)
    {
        m_pVideoES2YUV = new AVSES2YUV();
        if(NULL == m_pVideoES2YUV)
        {
            printf("Channel::ReStartVideoES2YUV Failed\n");
            return false;
        }
    }
    else
    {
        m_pVideoES2YUV = new IntelES2YUV(m_stTSInParam.nVStreamType, m_stTSOutParam.nWidth, m_stTSOutParam.nHeight, m_stTSOutParam.nFrameRate);
        if (NULL == m_pVideoES2YUV)
        {
            printf("Channel::ReStartVideoES2YUV Failed\n");
            return false;
        }
    }
    m_pVideoES2YUV->RegisterCallbackFunc(S_WriteYUVData, this);

    return true;
}

bool Channel::ReStartVideoYUV2ES()
{
    log_info("restart channel %d", m_nIndex);
    // Stop
    SAFE_DELETE(m_pVideoYUV2ES);

    // Start
    m_pVideoYUV2ES = new VideoYUV2ES(m_stTSOutParam);
    if (NULL == m_pVideoYUV2ES)
    {
        return false;
    }

    // 重启OSD模块
    SAFE_DELETE(m_pOsdMonitor);

    m_pOsdMonitor = new COsdMonitor;
    if (m_pOsdMonitor)
    {
        m_pOsdMonitor->Init(m_stTSOutParam.nWidth, m_stTSOutParam.nHeight, m_stTSOutParam.nFrameRate);
        m_pOsdMonitor->SetOsdInfo(m_stOSD);
    }

    return true;
}

bool Channel::ReStartAudioES2PCM()
{
    log_info("restart channel %d", m_nIndex);
    // Stop
    SAFE_DELETE(m_pAudioES2PCM);

    // Start
    if(AV_CODEC_A_DRA == m_stTSInParam.nAStreamType)
    {
        m_pAudioES2PCM = new DraES2PCM();
        if(NULL == m_pAudioES2PCM)
        {
            printf("Channel::ReStartAudioES2PCM(DraES2PCM) Failed\n");
            return false;
        }
    }
    else
    {
        m_pAudioES2PCM = new FFMPEGES2PCM();
        if(NULL == m_pAudioES2PCM)
        {
            printf("Channel::ReStartAudioES2PCM(FFMPEGES2PCM) Failed\n");
            return false;
        }
    }
    m_pAudioES2PCM->Init(m_stTSInParam.nAStreamType);
    m_pAudioES2PCM->RegisterCallbackFunc(S_WritePCMData, this);

    return true;
}

bool Channel::ReStartAudioPCM2ES()
{
    log_info("restart channel %d", m_nIndex);
    // Stop
    SAFE_DELETE(m_pAudioPCM2ES);

    // Start
    m_pAudioPCM2ES = new AudioPCM2ES();
    if(NULL == m_pAudioPCM2ES)
    {
        printf("Channel::ReStartAudioPCM2ES Failed\n");
        return false;
    }
    m_pAudioPCM2ES->Init(m_stTSOutParam.nAStreamType, m_stTSOutParam.nARate);

    return true;
}

bool Channel::HaveBlackFrameData()
{
    // Black Frame
    int nBlackFrameYSize = m_stTSOutParam.nWidth * m_stTSOutParam.nHeight;
    int nBlackFrameUVSize = nBlackFrameYSize / 2;

    if(nBlackFrameYSize != m_nBlackFrameYSize)
    {
        m_nBlackFrameYSize = nBlackFrameYSize;

        SAFE_FREE(m_pBlackFrameYAddr);
        m_pBlackFrameYAddr = (unsigned char *)malloc(m_nBlackFrameYSize);

        if(NULL == m_pBlackFrameYAddr)
            return false;  
    }

    if(nBlackFrameUVSize != m_nBlackFrameUVSize)
    {
        m_nBlackFrameUVSize = nBlackFrameUVSize;

        SAFE_FREE(m_pBlackFrameUVAddr);
        m_pBlackFrameUVAddr = (unsigned char *)malloc(m_nBlackFrameUVSize);
        memset(m_pBlackFrameUVAddr, 120, m_nBlackFrameUVSize);

        if(NULL == m_pBlackFrameUVAddr)
            return false;
    }

    return true;
}

Tool::TH_RET Channel::TH_Work(LPVOID lpContext)
{
    if(lpContext == NULL)
        return NULL;
    Channel *pContext = (Channel *)lpContext;
    pContext->Work();

    return NULL;
}

int Channel::Work()
{
    unsigned long long ullCurTime = OS_SysTime_Ms();

    m_ullLastRecvVideoTime = ullCurTime;
    m_ullLastRecvAudioTime = ullCurTime;
    m_ullLastSendBlackFrameTime = ullCurTime;

    while (m_bRun)
    {
        ullCurTime = OS_SysTime_Ms();

        // 检查是否需要重启某些模块
        CheckReStartFlag();

        // 记录监测状态
        MonitorStatusRecord(ullCurTime);

        // 视频转码监测处理
        bool bHaveVideoData = VideoDataProcess();
        if (bHaveVideoData)
        {
            m_ullLastRecvVideoTime = ullCurTime;
        }
        else
        {
            if (ullCurTime - m_ullChannelStartTime > 3000)
            {
                // 视频丢失监测(码流中断)
                if ((int)(ullCurTime - m_ullLastRecvVideoTime) > m_objMonitor.m_stThrldVideo.nLostTime)
                {
                    m_stMonitorWarnStatus.sWarnVideoStatus = (1 << WARN_TYPE_V_LOST);

                    if ((int)(ullCurTime - m_ullLastSendBlackFrameTime) > g_nBlackSendInterval)
                    {
                        m_ullLastSendBlackFrameTime = ullCurTime;

                        // 视频码流中断 输出黑场
                        if (HaveBlackFrameData())
                        {
                            // YAddr置黑
                            for (int i = 0; i < m_stTSOutParam.nHeight; ++i)
                            {
                                memset(m_pBlackFrameYAddr + i*m_stTSOutParam.nWidth, 0, m_stTSOutParam.nWidth);
                            }

                            m_ullLastVedioPTS += g_nBlackPTSIncrement;
                            VideoDataProcess2(m_ullLastVedioPTS, g_nBlackFrameRate);
                        }
                    }
                }
                else
                {
                    m_stMonitorWarnStatus.sWarnVideoStatus &= ~(1 << WARN_TYPE_V_LOST);
                }
            }
        }

        // 音频转码监测处理
        bool bHaveAudioData = AudioDataProcess();
        if (bHaveAudioData)
        {
            m_ullLastRecvAudioTime = ullCurTime;
        }
        else
        {
            if (ullCurTime - m_ullChannelStartTime > 3000)
            {
                // 音频丢失监测(码流中断)
                if ((int)(ullCurTime - m_ullLastRecvAudioTime) > m_objMonitor.m_stThrldAudio.nLostTime)
                {
                    m_stMonitorWarnStatus.sWarnAudioStatus = (1 << WARN_TYPE_A_LOST);
                }
                else
                {
                    m_stMonitorWarnStatus.sWarnAudioStatus &= ~(1 << WARN_TYPE_A_LOST);
                }
            }
        }

        if (!(bHaveVideoData || bHaveAudioData))
        {
            OS_Thread::Msleep(10);
        }
    }

    return 0;
}

bool Channel::VideoDataProcess()
{
    // 1.获取一帧ES数据 PTS
    FrameBitStream stFrame;
    int nScrambleFlag = 0;
	bool getESData = false;

	if (!m_pStreamReader->ReadVFrame(stFrame.pData, stFrame.uLen, stFrame.ullPts, nScrambleFlag, getESData))
    {
        if (1 == nScrambleFlag)
            m_stMonitorWarnStatus.sWarnVideoStatus = (1 << WARN_TYPE_V_SCRAMB);
		else
		{
			m_stMonitorWarnStatus.sWarnVideoStatus &= ~(1 << WARN_TYPE_V_SCRAMB);
		}

		if (getESData)
		{
			return true;
		}
        return false;
    }

    // 视频加扰异常 不解码
    if (1 == nScrambleFlag)
    {
        m_stMonitorWarnStatus.sWarnVideoStatus = (1 << WARN_TYPE_V_SCRAMB);
        return true;
    }
	else
	{
		m_stMonitorWarnStatus.sWarnVideoStatus &= ~(1 << WARN_TYPE_V_SCRAMB);
	}

    // 2.ES数据到YUV数据解析(IMSDK)
    // 视频解码异常监测
    if (!m_pVideoES2YUV->Decode(stFrame))
    {
        m_stMonitorWarnStatus.sWarnVideoStatus = (1 << WARN_TYPE_V_DEC);
    }
	else
	{
		m_stMonitorWarnStatus.sWarnVideoStatus &= ~(1 << WARN_TYPE_V_DEC);
	}

    return true;
}

void Channel::S_WriteYUVData(void *pSurface, unsigned long long ullPTS, int nFrameRate, void *pContext)
{
    Channel *pChannel = (Channel *)pContext;
    if (pChannel && pChannel->m_bRun)
    {
        // 记录最近一次的视频PTS
        pChannel->m_ullLastVedioPTS = ullPTS;

        if (AV_CODEC_V_AVS != pChannel->m_stTSInParam.nVStreamType)
        {
            pChannel->VideoDataProcess2((mfxFrameSurface1 *)pSurface, ullPTS);            
        }
        else
        {
            pChannel->VideoDataProcess2((AVSFrame *)pSurface, ullPTS, nFrameRate);
        }
    }

    return ;
}

void Channel::VideoDataProcess2(mfxFrameSurface1 *pSurface, unsigned long long ullPTS)
{
    // 3.YUV数据监测(黑场 彩场 彩条 静帧)
    if (OS_SysTime_Ms() - m_ullChannelStartTime > 3000)
    {
        // 注意：这里视频Width和Height时  不能传入pSurface->Info.CropW/CropH  Width需要传入pSurface->Data.Pitch  但是有效数据宽度仍是pSurface->Data.Width
        m_objMonitor.VideoDataMonitor(pSurface->Data.Y + (pSurface->Info.CropY * pSurface->Data.Pitch + pSurface->Info.CropX), 
            pSurface->Data.Cr, pSurface->Data.Pitch, pSurface->Info.CropH, m_stMonitorWarnStatus.sWarnVideoStatus);
        // UV值传入CR指针
    }

    // 4.OSD叠加
#if 1
    if (m_pOsdMonitor)
        m_pOsdMonitor->DrawOsdUseFfmpeg(pSurface);
#else
	m_objOsdMonitor.DrawOsd(pSurface->Data.Y + pSurface->Info.CropY * pSurface->Data.Pitch + pSurface->Info.CropX, 
		pSurface->Info.CropW, pSurface->Info.CropH, pSurface->Data.Pitch);
#endif

    // 5.YUV数据编码成为ES(IMSDK) (输入帧率是已经VPP过后的 所以编码时帧率已经是目标帧率了)
    if (!m_pVideoYUV2ES->Init(m_stTSOutParam.nFrameRate, m_stTSOutParam.nWidth, m_stTSOutParam.nHeight))
    {
        return ;
    }

    unsigned char *pData = NULL;
    unsigned int uLen = 0;

    m_pVideoYUV2ES->Encode(pSurface, pData, uLen);
    if (NULL == pData || 0 == uLen)
    {
        return;
    }

    // 6.ES封装为TS
    m_pStreamWriter->WriteNextFrame(pData, uLen, ullPTS, m_stTSOutParam.nVStreamType, m_stTSInParam.nVPID);
}

void Channel::VideoDataProcess2(AVSFrame *pAVSFrame, unsigned long long ullPTS, int nFrameRate)
{
    // 3.将原始YUV数据通过VideoYUV2ES模块转换为pSurface结构体
    if (! (m_pVideoYUV2ES->Init(nFrameRate, pAVSFrame->nWidth, pAVSFrame->nHeight)))
    {
        return;
    }

    mfxFrameSurface1 *pSurface = m_pVideoYUV2ES->Package(pAVSFrame->data[0], pAVSFrame->linesize[0], pAVSFrame->data[1], pAVSFrame->linesize[1], pAVSFrame->data[2], pAVSFrame->linesize[2]);
    if(NULL == pSurface) return; 

    // 4.YUV数据监测(黑场 彩场 彩条 静帧)
    if (OS_SysTime_Ms() - m_ullChannelStartTime > 3000)
    {
        // 注意：这里视频Width和Height时  不能传入pSurface->Info.CropW/CropH  Width需要传入pSurface->Data.Pitch  但是有效数据宽度仍是pSurface->Data.Width
        m_objMonitor.VideoDataMonitor(pSurface->Data.Y + (pSurface->Info.CropY * pSurface->Data.Pitch + pSurface->Info.CropX), 
            pSurface->Data.Cr, pSurface->Data.Pitch, pSurface->Info.CropH, m_stMonitorWarnStatus.sWarnVideoStatus);
        // UV值传入CR指针
    }

    // 5.OSD叠加
#if 1
    if (m_pOsdMonitor)
        m_pOsdMonitor->DrawOsdUseFfmpeg(pSurface);
#else
    m_objOsdMonitor.DrawOsd(pSurface->Data.Y + pSurface->Info.CropY * pSurface->Data.Pitch + pSurface->Info.CropX, 
        pSurface->Info.CropW, pSurface->Info.CropH, pSurface->Data.Pitch);
#endif


    // 6.YUV数据编码成为ES(IMSDK)
    unsigned char *pData = NULL;
    unsigned int uLen = 0;

    // 这里需要使用 VideoYUV2ES的VPP功能
    m_pVideoYUV2ES->Encode(pSurface, pData, uLen, true);
    if (NULL == pData || 0 == uLen)
    {
        return;
    }

    // 7.ES封装为TS
    m_pStreamWriter->WriteNextFrame(pData, uLen, ullPTS, m_stTSOutParam.nVStreamType, m_stTSInParam.nVPID);
}

// 黑场帧输出
void Channel::VideoDataProcess2(unsigned long long ullPTS, int nFrameRate)
{
    // 3.将原始YUV数据通过VideoYUV2ES模块转换为pSurface结构体
    if (!(m_pVideoYUV2ES->Init(nFrameRate, m_stTSOutParam.nWidth, m_stTSOutParam.nHeight)))
    {
        return;
    }

    mfxFrameSurface1 *pSurface = m_pVideoYUV2ES->Package(m_pBlackFrameYAddr, m_stTSOutParam.nWidth, m_pBlackFrameUVAddr, m_stTSOutParam.nWidth, m_pBlackFrameUVAddr, m_stTSOutParam.nWidth);
    if(NULL == pSurface) return;

    // 4.OSD叠加
#if 1
    if (m_pOsdMonitor)
        m_pOsdMonitor->DrawOsdUseFfmpeg(pSurface);
#else
    m_objOsdMonitor.DrawOsd(pSurface->Data.Y + pSurface->Info.CropY * pSurface->Data.Pitch + pSurface->Info.CropX, 
        pSurface->Info.CropW, pSurface->Info.CropH, pSurface->Data.Pitch);
#endif


    // 5.YUV数据编码成为ES(IMSDK)
    unsigned char *pData = NULL;
    unsigned int uLen = 0;
    // 这里不需要使用VideoYUV2ES的VPP功能  但是需要使用VPP申请的内存
    m_pVideoYUV2ES->Encode(pSurface, pData, uLen);
    if(NULL == pData || 0 == uLen)
    {
        return ;
    }

    // 6.ES封装为TS
    m_pStreamWriter->WriteNextFrame(pData, uLen, ullPTS, m_stTSOutParam.nVStreamType, m_stTSInParam.nVPID);
}

bool Channel::AudioDataProcess()
{
    // 1.获取一帧ES数据 PTS
    FrameBitStream stFrame;
    if (!m_pStreamReader->ReadAFrame(stFrame.pData, stFrame.uLen, stFrame.ullPts))
    {
        return false;
    }

    // 2.ES数据到YUV数据解析(FFMPEG)
    // 解码之后的数据，有几通道就输出几通道
    m_pAudioES2PCM->Decode(stFrame.pData, stFrame.uLen, stFrame.ullPts, stFrame.ullPts);

    return true;
}

void Channel::S_WritePCMData(unsigned char *pPcmData, unsigned int uLen, unsigned long long ullPTS, unsigned int uSamplesNumber, void *pContext)
{
    Channel *pChannel = (Channel *)pContext;
    if (pChannel && pChannel->m_bRun)
    {
        pChannel->AudioDataProcess2(pPcmData, uLen, ullPTS, uSamplesNumber);
    }

    return;
}

void Channel::AudioDataProcess2(unsigned char *pPcmData, unsigned int uPcmDataLen, unsigned long long ullPTS, unsigned int uSamplesNumber)
{
    // 3.PCM数据监测
    if (OS_SysTime_Ms() - m_ullChannelStartTime > 3000)
    {
        m_objMonitor.AudioDataMonitor(pPcmData, AUDIO_SWR_CHANNELS, uSamplesNumber, AUDIO_SWR_BIT_SAMPLE, m_stMonitorWarnStatus.sWarnAudioStatus);
    }

    // 4.PCM数据编码成为ES(FFMPEG)
    m_pAudioPCM2ES->InputEncodeData(pPcmData, uPcmDataLen, ullPTS);

    while (1)
    {
        unsigned char *pData = NULL;
        unsigned int uLen = 0;
        
        if (!m_pAudioPCM2ES->Encode(pData, uLen))
        {
            return;
        }

        // 5.ES封装为TS
        m_pStreamWriter->WriteAudioData(pData, uLen, ullPTS, m_stTSOutParam.nAStreamType, m_stTSInParam.vecAPID[0]);
    }
}

bool Channel::GetWarnStatus(unsigned short &sWarnVedioStatus, unsigned short &sWarnAudioStatus, string &strOutIP, int &nOutPort)
{
    if(m_bRun && 0 != m_stMonitorOutAddr.nSwitch)
    {
        //sWarnVedioStatus = m_sLastWarnVideoStatus;
        //sWarnAudioStatus = m_sLastWarnAudioStatus;

        sWarnVedioStatus = m_stMonitorWarnStatus.sWarnVideoStatus;
        sWarnAudioStatus = m_stMonitorWarnStatus.sWarnAudioStatus;

        strOutIP = m_stMonitorOutAddr.strIP;
        nOutPort = m_stMonitorOutAddr.nPort;

        return true;
    }

    return false;
}

bool Channel::MonitorStatusRecord(unsigned long long ullCurTime)
{
    // 不在此处判断监测时间间隔 在协议上报出进行判断 否则容易造成此处和音视频监测时间间隔错开 导致监测不到任何告警
    //m_sLastWarnVideoStatus = m_stMonitorWarnStatus.sWarnVideoStatus;

#if 1
    if (m_pOsdMonitor)
        m_pOsdMonitor->CatchVideoMonitor(m_stMonitorWarnStatus.sWarnVideoStatus);
#else
    m_objOsdMonitor.CatchVideoMonitor(m_sLastWarnVideoStatus);
#endif

    //m_sLastWarnAudioStatus = m_stMonitorWarnStatus.sWarnAudioStatus;

#if 1
    if (m_pOsdMonitor)
        m_pOsdMonitor->CatchAudioMonitor(m_stMonitorWarnStatus.sWarnAudioStatus);
#else
    m_objOsdMonitor.CatchAudioMonitor(m_sLastWarnAudioStatus);
#endif

    return true;
}

void Channel::CheckReStartFlag()
{
    if (0 == m_cParamReset) return;

    m_mutexParamReset.Lock();
    char cParamResetTmp = m_cParamReset;
    m_cParamReset = 0;
    m_stTSInParam = m_stTSInParamProtocol;
    m_stTSOutParam = m_stTSOutParamProtocol;
    m_mutexParamReset.Unlock();

    for (int i = 0; i < RESTART_MAX_INDEX; ++i)
    {
        if (1 == ((cParamResetTmp >> i) & 0x01))
        {
            switch (i)
            {
            case RESTART_TSOUT:
                {
                    ReStartTSOut();
                    break;
                }
            case RESTART_TSIN:
                {
                    ReStartTSIn();
                    break;
                }
            case RESTART_VIDEO_ES2YUV:
                {
                    ReStartVideoES2YUV();
                    break;
                }
            case RESTART_VIDEO_YUV2ES:
                {
                    ReStartVideoYUV2ES();
                    break;
                }
            case RESTART_AUDIO_ES2PCM:
                {
                    ReStartAudioES2PCM();
                    break;
                }
            case RESTART_AUDIO_PCM2ES:
                {
                    ReStartAudioPCM2ES();
                    break;
                }
            default:
                {
                    printf("ReStart Index:%d Error\n", i);
                    break;
                }
            }// end of switch
        }
    }
}

