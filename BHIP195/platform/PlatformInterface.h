#pragma once

#include "Common.h"
#include "Markup.h"					//xml process
#include "BHXml.h"
#include "OSMutex.h"
#include "OSSocket.h"
#include "OSTime.h"
#include "ZThread.h"
#include "HttpStation.h"
#include "SysConfig.h"

#include "RingBuf.h"
#include "RingBuffer.h"

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;

//64位的时钟，从1900年1月1日开始的描述
struct NtpTimePacket
{
	unsigned int m_unInteger;			//整数部分，单位s
	unsigned int m_unFractional;		//小数部分
};

//SNTP协议节点节点
typedef struct SntpNode 
{
	unsigned char Mode	: 3;		
	unsigned char VN	: 3;
	unsigned char LI	: 2;

	unsigned char Stratum;
	unsigned char Poll;
	char Precision;
	float RootDelay;
	float RootDispersion;
	char RefIndentifier[4];
	struct NtpTimePacket RefTimestamp;				//最后一次校正的时间
	struct NtpTimePacket OrgTimestamp;				//
	struct NtpTimePacket ReceiveTimestamp;			//接收时间
	struct NtpTimePacket TransTimestamp;			//发送时间
}SntpNode;

class PlatformInterface;

struct ToPlatformMsg;
typedef bool (PlatformInterface::*pfnDoCmd)(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
typedef std::map<string, pfnDoCmd>	DoCmdFnMap; 

class PlatformInterface: public CXMLParser
{
public:
    PlatformInterface(void);
	~PlatformInterface(void);
	
    static PlatformInterface *GetInstance();
    static void DelInstance();

	bool Start();
	void Stop();

    void ParseXML(const string& strXML);

	// 添加发送给平台的协议消息
    void AddMsgToPlatform(const char * strXml, string &strPlatformIP, int nPlatformPort);
	int SntpRecv(unsigned char *pData,struct SntpNode *pSendNode);
	int SntpCreatSocket(string strIP);
	
private:
    // 工作线程
	static Tool::TH_RET TH_Work(LPVOID lpContext);
	void Work();

	//校时服务器
	static Tool::TH_RET TH_Sntp(LPVOID lpContext);
	void* SntpProcess();

	//板卡升级
	static Tool::TH_RET UpdateCmdCreateThread(LPVOID lpContext);

	// Update子线程，接收升级包后，调用process
	void *UpdateCommunication();
	int  process();

    void InitCRC();
    void CalcCRC(unsigned char *pData, unsigned int uDataLen);

private:
    // HTTP命令交互协议
    void RegParseInterface();

    bool CmdGetDeviceInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdSetOutputInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdGetOutputInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdUpdate(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdRestart(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdFactory(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdSaveAllCfg(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdSetTimeServerAddr(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdSetTcorderInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdGetTcorderInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdSetOSDInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);
    bool CmdGetOSDInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort);

    // 重启开机后 检查是否需要上报协议
    void RestartReport();

private:
    // 二进制组播主动推送协议
    void MultiBoardCheck(const SysConfig &stSysConfig);
    void MultiChannelResource(const SysConfig &stSysConfig);
    void MultiAVErrorStatus(const SysConfig &stSysConfig);

private:
    bool	        m_bRun;

    Tool::TH_HANDLE	m_hWorkThread;			// 工作线程

	Tool::TH_HANDLE	m_hSntpThread;			// 校时服务器线程  SntpProcess

	Tool::TH_HANDLE	m_hUpdateThread;		//升级线程
	
    DoCmdFnMap	m_mapDoCmdFn;
    HttpStation::CHttpStation   m_objHttpStation;
	
    OS_Mutex                          m_mutexRingBuf;
    CRingBuffer<ToPlatformMsg, 200>	  m_ringToPlatform;

private:
    OS_UdpSocket    m_sockMulitPost;        // 组播推送socket
    unsigned int	m_arrCrc32Table[256];   // CRC32校验

	int             m_SocketFd;	            //校时套接字
	char            m_byaServerIp[32];	
	int             m_SNTPPort;	            //SNTP端口
	unsigned int    m_uTimeout10S_Count; 
	unsigned int    m_uUpdateTime;	        //同步时间
	unsigned int    m_uGap;		            //10 min

};

struct ToPlatformMsg
{
    string  strMsg;

    string  strPlatformIP;
    int     nPlatformPort;

    ToPlatformMsg(): strMsg(""), strPlatformIP(""), nPlatformPort(0)
    {}

    ToPlatformMsg(string msg, string platformIP, int platformPort): strMsg(msg), strPlatformIP(platformIP), nPlatformPort(platformPort)
    {}
};

