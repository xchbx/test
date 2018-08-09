#include "ChannelManager.h"
#include "SysConfig.h"
#include "public/FileEx.h"


extern map<string, CUdpSendManager *> g_mapUdpSendManager;
extern map<string, CUdpRecvManager *> g_mapUdpRecvManager;

static ChannelManager *g_pChannelManager = NULL;

#if 0
extern char *g_dataHZK16;
extern char *g_dataHZK48;
extern char *g_dataASC16;
extern char *g_dataASC48;
#endif

ChannelManager::ChannelManager(void)
{
    m_arrVCode2StreamType[0] = AV_CODEC_UNKNOWN;
    m_arrVCode2StreamType[1] = AV_CODEC_V_MPEG2;
    m_arrVCode2StreamType[2] = AV_CODEC_V_H264;
    m_arrVCode2StreamType[3] = AV_CODEC_V_MPEG4;
    m_arrVCode2StreamType[4] = AV_CODEC_V_AVS;
    m_arrVCode2StreamType[5] = AV_CODEC_V_HEVC;

    m_arrACode2StreamType[0] = AV_CODEC_UNKNOWN;
    m_arrACode2StreamType[1] = AV_CODEC_A_AAC;
    m_arrACode2StreamType[2] = AV_CODEC_A_AC3;
    m_arrACode2StreamType[3] = AV_CODEC_A_MPEG2;
    m_arrACode2StreamType[4] = AV_CODEC_A_DRA;

#if 0
	g_dataHZK16 = CaptureLoadFile(LATTICE_LIB_PATH_HZK_16, g_dataHZK16);
	g_dataHZK48 = CaptureLoadFile(LATTICE_LIB_PATH_HZK_48, g_dataHZK48);
	g_dataASC16 = CaptureLoadFile(LATTICE_LIB_PATH_ASC_16, g_dataASC16);
	g_dataASC48 = CaptureLoadFile(LATTICE_LIB_PATH_ASC_48, g_dataASC48);
#endif
}

ChannelManager::~ChannelManager(void)
{
    DeInit();
#if 0
	CaptureFreeFile(&g_dataHZK16);
	CaptureFreeFile(&g_dataHZK48);
	CaptureFreeFile(&g_dataASC16);
	CaptureFreeFile(&g_dataASC48);
#endif
}

ChannelManager *ChannelManager::GetInstance()
{
    if (g_pChannelManager == NULL)
    {
        g_pChannelManager = new ChannelManager();
    }

    return g_pChannelManager;
}

void ChannelManager::DelInstance()
{
    if (g_pChannelManager != NULL)
    {
        SAFE_DELETE(g_pChannelManager);
    }
}

#if 0
void ChannelManager::CaptureFreeFile(char ** pBuf)
{
	if (pBuf != NULL && *pBuf != NULL)
	{
		free(*pBuf);
		pBuf=NULL;
	}
}

char * ChannelManager::CaptureLoadFile(char * fileName, char * pBuf)
{		
	CFileEx::SetCurDirectory(CFileEx::GetExeDirectory().c_str());

	FILE *fp = fopen(fileName, "rb");
	if ( fp == NULL ) 	
	{
		printf("open file: %s failed !!\r\n", fileName);
		return NULL;
	}	

	fseek(fp,0L,SEEK_END);  
	int fileSize = ftell(fp);

	pBuf = (char *)malloc(fileSize);
	if(pBuf == NULL)
	{
		printf("[osd_link] malloc failed!\r\n");
		fclose(fp);
		return NULL;
	}

	fseek(fp, 0, SEEK_SET);

	if(fread(pBuf, 1, fileSize, fp) <= 0)
	{
		printf("read file: %s failed !!\r\n", fileName);
		free(pBuf);
		fclose(fp);
		return NULL;
	}

	fclose(fp);	

	return pBuf;
}
#endif

bool ChannelManager::Init()
{
    SysConfig &stSysConfig = CConfig::GetInstance()->GetConfig();
    for (auto it = stSysConfig.mapNetInfo.begin(); it != stSysConfig.mapNetInfo.end(); ++it)
    {
        // ToDo 数据网卡开启线程
        string strNetIP = it->second.strIP;

        // UDP发送
        CUdpSendManager * pUdpSendManager = new CUdpSendManager;
        if (pUdpSendManager)
        {
            pUdpSendManager->StartUdpSendManager(strNetIP.c_str());
            g_mapUdpSendManager[strNetIP] = pUdpSendManager;
        }
        else
        {
            log_error("Init UdpSend NetIP:%s Failed", strNetIP.c_str());
            return false;
        }

        // UDP接收
        CUdpRecvManager * pUdpRecvManager = new CUdpRecvManager;
        if (pUdpRecvManager)
        {
            pUdpRecvManager->StartUdpRecvManager(strNetIP.c_str());
            g_mapUdpRecvManager[strNetIP] = pUdpRecvManager;
        }
        else
        {
            log_error("Init UdpRecv NetIP:%s Failed", strNetIP.c_str());
            return false;
        }
    }

    for (int i = 0; i < CHANNEL_MAX_NUM; ++i)
    {
        // 将配置文件中的参数放入
        SysChannel &stSysChannel = CConfig::GetInstance()->GetChannel(i);
        ProcessTSParamStreamType(stSysChannel.stTSInParam, stSysChannel.stTSOutParam);

        m_arrChannel[i] = new Channel(stSysChannel);
        if (NULL == m_arrChannel[i])
        {
            log_error("Init Index:%d Failed\n", i);
            return false;
        }

        if (stSysChannel.bStart)
        {
            m_arrChannel[i]->Start();
            log_info("Index:%d Start Success\n", i);
        }
    }

    return true;
}

void ChannelManager::DeInit()
{
    for (int i = 0; i < CHANNEL_MAX_NUM; ++i)
    {
        SAFE_DELETE(m_arrChannel[i]);
    }

    auto itUdpSend = g_mapUdpSendManager.begin();
    for ( ; itUdpSend != g_mapUdpSendManager.end(); itUdpSend++)
    {
        SAFE_DELETE(itUdpSend->second);
    }
    g_mapUdpSendManager.clear();

    auto itUdpRecv = g_mapUdpRecvManager.begin();
    for ( ; itUdpRecv != g_mapUdpRecvManager.end(); itUdpRecv++)
    {
        SAFE_DELETE(itUdpRecv->second);
    }
    g_mapUdpRecvManager.clear();
}

void ChannelManager::SetChannelMultiAddr(int nIndex, TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr) 
{
	m_arrChannel[nIndex]->SetMultiAddr(stTSOutAddr, stMonitorOutAddr); 

	if (1 == stTSOutAddr.nSwitch)
	{
        m_arrChannel[nIndex]->Start();
        printf("ChannelManager::SetChannelMultiAddr Index:%d Start Success\n", nIndex);
	}
	else if (0 == stTSOutAddr.nSwitch)
	{
		m_arrChannel[nIndex]->Stop();
        printf("ChannelManager::SetChannelMultiAddr Index:%d Stop Success\n", nIndex);
	}
}

bool ChannelManager::SetChannelTransParam(vector<int> &vecChannelID, TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam)
{
    if (vecChannelID.empty())
    {
        printf("ChannelManager::SetChannelTransParam vecChannelID empty\n");
        return false;
    }

    ProcessTSParamStreamType(stTSInParam, stTSOutParam);

    for(auto it=vecChannelID.begin(); it!=vecChannelID.end(); ++it)
    {
        int nIndex = *it;
        m_arrChannel[nIndex]->SetTransParam(stTSInAddr, stTSInParam, stTSOutParam); 
    }

    // 转码卡默认将转出来的码流数据和分析数据使用第一个通道输出(文档指定)
    //int nIndex = vecChannelID[0];
    //m_arrChannel[nIndex]->Start();
    //printf("ChannelManager::StartChannel Index:%d Start Success\n", nIndex);

    return true;
}

bool ChannelManager::ReStartChannel(int nIndex)
{
    m_arrChannel[nIndex]->Stop();
    m_arrChannel[nIndex]->Start();
    printf("ChannelManager::StartChannel Index:%d ReStart Success\n", nIndex);

    return true;
}

int ChannelManager::GetFreeChannel()
{
    for(int i=0; i<CHANNEL_MAX_NUM; ++i)
    {
        if(false == m_arrChannel[i]->GetRunStatus())
        {
            return i;
        }
    }
    
    return -1;
}

void ChannelManager::ProcessTSParamStreamType(TSInParam &stTSInParam, TSOutParam &stTSOutParam)
{
    stTSInParam.nVStreamType = m_arrVCode2StreamType[stTSInParam.nVCode];
    stTSInParam.nAStreamType = m_arrACode2StreamType[stTSInParam.nACode];

    stTSOutParam.nVStreamType = m_arrVCode2StreamType[stTSOutParam.nVCode];
    stTSOutParam.nAStreamType = m_arrACode2StreamType[stTSOutParam.nACode];
}

