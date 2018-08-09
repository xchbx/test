#ifndef _HTTP_STATION_H_
#define _HTTP_STATION_H_

#include <vector>
#include <list>
#include <map>
using namespace std;

#include "OSSocket.h"
#include "Thread.h"
#include "OSMutex.h"
#include "RingBuffer.h"
#include "XMLParser.h"



namespace HttpStation
{
#define MAX_RECV_TH_NUM		50

#define REAPET_CHECK_NUM	0X0FFF
//#define REAPET_CHECK_NUM	0X000F

//接收到HTTP消息的回调函数
typedef void (*HttpRecvCallBack) (
	const char* strContent,		//收到的http数据内容
	const char* szFrom,			//发送数据的源地址
	int nPort,					//发送数据的源端口
	void *lp					//上下文参数
	);

typedef void (*GetXMLParserCallBack)(void* lp, const string& strCode, CXMLParser*& pObj);

typedef std::map<int, int>	MSGIDMAP;

//处理接收的线程对象
class CHttpRecvTH : public OS_Thread
{
public:
	CHttpRecvTH();
	CHttpRecvTH(const OS_TcpSocket& sock);
	~CHttpRecvTH();

public:
	void RegFun(void *lp, GetXMLParserCallBack pFn);

	int Start();
	bool IsExit();
	void SetExit();

	bool IsSame(const std::string& strFromIP, int nFromPort);
	bool IsSame(const std::string& strFromCode);

	void SetSockTimeOut(int nSec, bool bTmOutCut = true);

private:
	void Stop();

	int Routine(TH1);

	//处理接收信息
	void ProcessMsg(OS_TcpSocket& sock);

	bool ParseHttpMsg(std::string& strMsg, std::string& strHttp);

	void GenerateOKReply();

	bool ParseMsgID(const std::string& strMsg, std::string& strMsgID);
	bool ParseSrcCode(const std::string& strMsg, std::string& strSrcCode);
	bool ParseDstCode(const std::string& strMsg, std::string& strSrcCode);
	void CreateReplyXml(const std::string& strMsgID, std::string& strMsg);
	void WriteLog(std::string& strMsg);

private:
	//线程状态标记
	bool	m_bStop;
	bool	m_bExitFlag;
  
	//数据收发的上下文参数
	CXMLParser*	m_pParseObject;
	bool m_bParseObjEffect;

	//待接Socket
	OS_TcpSocket m_RecvSock;

	std::string m_strOKReply;

	std::string m_strFromIP;
	int	m_nFromPort;

	std::string m_strFromCode;

	GetXMLParserCallBack m_pFnGetXMLParser;
	void*	m_lpContext;

	int	         m_pBufMsgID[REAPET_CHECK_NUM];

	FILE*        m_pRecvSwapFile;
	int          m_nLine;
};
typedef std::vector<CHttpRecvTH*>	PHttpRecvTHVec;

//处理发送的线程对象
//发送状态通知回调函数
typedef void (*HttpSendCallBack) (
								  const std::string& strToIP,	//目标地址
								  int nToPort,
								  int nStatus,					//发送状态
								  void *lp						//上下文参数
								  );

typedef std::list<std::string>		PostMsgQue;

typedef std::vector<string>		MsgVec;


//<index, buf>
typedef std::map<unsigned int, CRingBuffer<std::string, 1000>*>	IndexBufMap;

extern class BufIndex g_objBufIndex;

class BufIndex
{
public:
	BufIndex();
	~BufIndex();

public:
	unsigned int Get();

private:
	unsigned int m_unBufIndex;
	OS_Mutex m_mutexBufIndex;
};

class CHttpSendTH : public OS_Thread
{
public:
	CHttpSendTH();
	CHttpSendTH(const std::string& strToIP, int nToPort, bool bLong = true);
	~CHttpSendTH();

public:
	//注册发送状态回调
	void RegCallBackFunc(HttpSendCallBack pfnHttpSend, void *lpContext);

	int Start();
	void Stop();
	void SetExit();
	bool IsExit();

	bool IsSame(const std::string& strToIP, int nToPort);

	void AddMsg(const std::string& strMsg, unsigned int nBufIndex = 0);
	void AddMsg(const MsgVec& vecMsg);
	
	unsigned int RegBufferUser();
	void RealeseBuf(unsigned int unBufIndex);
	void SetSendTimeout(int nTimeout);
	void SetPath(std::string &strPath);

private:
	bool Connect();
	int Routine(TH1);
	int PostMsg(const std::string& strMsg);
	bool Send(const std::string& strMsg);
	bool CheckReply(const std::string& strReply);
	bool StuffMsg(std::string &strContent, const std::string& strToIP, int nToPort); 
	void WriteLog(const std::string& strMsg);

private:
	//线程状态标记
	bool	m_bStop;
	bool	m_bExitFlag;

	std::string m_strPath;
	//发送状态通知回调函数
	HttpSendCallBack	m_pfnSendStatus;
	void*				m_lpContext;

	//发送Socket
	OS_TcpSocket	m_SendSock;
	bool			m_bSockConn;

	std::string m_strToIP;
	int			m_nToPort;

	IndexBufMap	m_mapIndexBuf;
	CRingBuffer<unsigned int, 1000> m_bufNeedDel;		//需要删除的BUF
	unsigned int m_unBufIndex;

	int	m_nSendTimeout;
	bool m_bLongConn;

	FILE* m_pSendSwapFile;
	int   m_nLine;
};
typedef std::vector<CHttpSendTH*>	PHttpSendTHVec;


class CHttpStation : public OS_Thread
{
public:
	CHttpStation();
	~CHttpStation();

public:
	void SetLocalAddr(const std::string strLocalIP = "", int nLocalPort = 0	);

	bool Start(void);
	void Stop(void);
	void SetExit();

	bool RegParseXmlObj(const std::string& strCode, CXMLParser* pObj);


	//发送相关
	bool PostMsg(const MsgVec& vecMsg, 
		const std::string& strToIP, 
		int nToPort, string strPath = "");

	CHttpSendTH* CreateSender(const std::string& strToIP, int nToPort, bool bLongConn = true);
	bool PostMsg(const std::string& strMsg, void* lpSend, unsigned int nBufIndex = 0);

	void RealeseSendBuf(void* lpSend, unsigned int nBufIndex);

	void DelRecvTH(const std::string& strFromIP, int nFromPort);
	void DelRecvTH(const std::string& strFromCode);

	void DelSendTH(const std::string& strToIP, int nToPort);


private:
	int Routine(TH1);
	int Routine(TH2);

	void GetTheXMLParser(const string& strCode, CXMLParser*& pObj);
	static void GetXMLParser(void* lp, const string& strCode, CXMLParser*& pObj);
	void CheckRepeatRecvTH(const std::string& strClientIP, int nClientPort);
	void Realese();

private:
	//本地地址和端口
	std::string		m_strIP;
	int				m_nPort;

	//监听套接字
	OS_TcpSocket m_listenSock;	

	//是否正在运行
	bool m_bStop; 

	//接收线程容器锁
	PHttpRecvTHVec m_vecPHttpRecvTH;
	OS_Mutex	m_mutexRecvTH;

	//发送线程容器锁
	PHttpSendTHVec m_vecPHttpSendTH;
	OS_Mutex	m_mutexSendTH;

	XMLParserMap	m_mapXMLParser;
	OS_Mutex	m_mutexXMLParserMap;
};
};
#endif

