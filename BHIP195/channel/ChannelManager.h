#ifndef CHANNEL_MANAGER_H_
#define CHANNEL_MANAGER_H_

#include "Common.h"
#include "Channel.h"
#include "ChannelStruct.h"


#define A_CODE_SUPPORT_NUM  6   // 5+1
#define V_CODE_SUPPORT_NUM  5   // 4+1

#define		LATTICE_LIB_PATH_HZK_16			"Config/lattice/HZK16S"
#define		LATTICE_LIB_PATH_ASC_16			"Config/lattice/ASC16"
#define		LATTICE_LIB_PATH_HZK_48			"Config/lattice/HZK48S"
#define		LATTICE_LIB_PATH_ASC_48			"Config/lattice/ASC48"

// 通道资源管理模块
class ChannelManager
{
public:
    ChannelManager(void);
    ~ChannelManager(void);

    static ChannelManager *GetInstance();
    static void DelInstance();

	//释放内存
	//void CaptureFreeFile(char ** pBuf);
	
	//加载文件
	//char* CaptureLoadFile(char * fileName, char* pBuf);
   
	bool Init();
    void DeInit();


public:
    // 重启通道
    bool ReStartChannel(int nIndex);

private:
    // 获取一个未使用Channel
    int GetFreeChannel();

    // 根据A/V Code修改TSParam中的A/V StreamType
    void ProcessTSParamStreamType(TSInParam &stTSInParam, TSOutParam &stTSOutParam);


public:
    // 业务相关  Channel Index需要减1 因为是从1开始的
    // 设置/获取通道转码参数  ToDo协议处理模块需要对参数进行基本检查
    bool SetChannelTransParam(vector<int> &vecChannelID, TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam);
    void GetChannelTransParam(int nIndex, TSInAddr &stTSInAddr, TSInParam &stTSInParam, TSOutParam &stTSOutParam)
        { m_arrChannel[nIndex]->GetTransParam(stTSInAddr, stTSInParam, stTSOutParam); }

    // 设置/获取通道OSD参数
    void SetChannelOSDParam(int nIndex, OsdInfo &stOSD) { m_arrChannel[nIndex]->SetOSDParam(stOSD); }
    void GetChannelOSDParam(int nIndex, OsdInfo &stOSD) { m_arrChannel[nIndex]->GetOSDParam(stOSD); }

    // 设置/获取通道输出组播地址及开关
    void SetChannelMultiAddr(int nIndex, TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr);
    void GetChannelMultiAddr(int nIndex, TSOutAddr &stTSOutAddr, MonitorOutAddr &stMonitorOutAddr) { m_arrChannel[nIndex]->GetMultiAddr(stTSOutAddr, stMonitorOutAddr); }

    bool GetMonitorStatus(int nIndex, unsigned short &sWarnVedioStatus, unsigned short &sWarnAudioStatus, string &strOutIP, int &nOutPort)
        {  return m_arrChannel[nIndex]->GetWarnStatus(sWarnVedioStatus, sWarnAudioStatus, strOutIP, nOutPort); }

private:
    // 板卡所有通道资源
    Channel *m_arrChannel[CHANNEL_MAX_NUM];

    int     m_arrACode2StreamType[A_CODE_SUPPORT_NUM];
    int     m_arrVCode2StreamType[V_CODE_SUPPORT_NUM];
};

#endif

