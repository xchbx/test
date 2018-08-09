#include "HttpStation.h"
#include "OSTime.h"
#include "Markup.h"
#include "Common.h"

#include <algorithm>


namespace HttpStation
{
//////////CHttpRecvTH
#define pcHttpHead "post / http"
#define pcContentLength "content-length"




// 一次性接收http数据的最大缓冲
const int	MAX_RECV_BUF = 8096;

CHttpRecvTH::CHttpRecvTH()
{
	m_pParseObject = NULL;
	m_bParseObjEffect = false;
	m_bExitFlag = true;
	m_bStop = false;
	m_strFromIP = "";
	m_nFromPort = 0;
	m_strFromCode = "";
	m_pFnGetXMLParser = NULL;
	m_lpContext = NULL;
	m_pRecvSwapFile = NULL;
	m_nLine = 0;
	memset(m_pBufMsgID, 0, sizeof(m_pBufMsgID)/sizeof(int));
	SetTHNum(1);
}

CHttpRecvTH::CHttpRecvTH(const OS_TcpSocket& sock)
{
	m_pParseObject = NULL;
	m_bParseObjEffect = false;
	m_bExitFlag = true;
	m_bStop = false;
	m_pFnGetXMLParser = NULL;
	m_lpContext = NULL;
	
	m_RecvSock = sock;
	m_RecvSock.SetOpt_RecvTimeout(5000);
	m_RecvSock.SetOpt_SendTimeout(5000);

	OS_SockAddr sockAddr;
	sock.GetPeerAddr(sockAddr);
	m_strFromIP = sockAddr.GetIp_str();
	m_nFromPort = sockAddr.GetPort();

	GenerateOKReply();
	memset(m_pBufMsgID, 0, sizeof(m_pBufMsgID)/sizeof(int));

	m_pRecvSwapFile = NULL;
	m_nLine = 0;
	SetTHNum(1);
}

CHttpRecvTH::~CHttpRecvTH()
{
	m_pFnGetXMLParser = NULL;
	m_lpContext = NULL;
	
	Stop();

	if (m_pRecvSwapFile)
	{
		fclose(m_pRecvSwapFile);
		m_pRecvSwapFile = NULL;
	}
}

void CHttpRecvTH::GenerateOKReply()
{
	m_strOKReply = "HTTP/1.0 200 OK\n";
	m_strOKReply += "\n";
	m_strOKReply += "Thu, 8 Nov 2012 15:54:30 GMT";
	m_strOKReply += "\n";
}

int CHttpRecvTH::Start()
{
	m_bStop = false;
	m_bExitFlag = false;

	if(0 != Run())
	{
		m_bExitFlag = true;
		m_bStop = true;
		Log("CHttpRecvTH::Start() Run() != 0");
		return -1;
	}

	return 0;
}

void CHttpRecvTH::Stop()
{
	m_bStop = true;	
	OS_Thread::Join(this);
}

void CHttpRecvTH::RegFun(void *lp, GetXMLParserCallBack pFn)
{
	m_lpContext = lp;
	m_pFnGetXMLParser = pFn;
}

bool CHttpRecvTH::IsExit()
{ 
	return m_bExitFlag; 
}

void CHttpRecvTH::SetExit()
{ 
	m_bStop = true;
}

bool CHttpRecvTH::IsSame(const std::string& strFromIP, int nFromPort)
{
	if (strFromIP == m_strFromIP && nFromPort == m_nFromPort)
	{
		return true;
	}

	return false;
}

bool CHttpRecvTH::IsSame(const std::string& strFromCode)
{
	if (strFromCode == m_strFromCode)
	{
		return true;
	}

	return false;
}

int CHttpRecvTH::Routine(TH1)
{
	//处理命令
	ProcessMsg(m_RecvSock);

	m_RecvSock.Close();
	m_bExitFlag = true;

	//设置通讯状态:断开
	if (m_bParseObjEffect && m_pParseObject)
	{
		m_pParseObject->SetCommStatus(false);
	}

	m_pParseObject = NULL;

	return 0;
}


typedef void (*ParseXML1)(const string& strXML);

void CHttpRecvTH::ProcessMsg(OS_TcpSocket& sock)
{
	std::string strContentBuffer(""), strMsg(""), strMsgID(""), strSrcCode("");
	char pcRecvBuf[MAX_RECV_BUF + 1] = {0};
	//int nSendLen(0);

	//fd_set tfdReadset;

	//struct timeval tTimeout;
	//tTimeout.tv_sec = 5;
	//tTimeout.tv_usec = 0;

	while (!m_bStop)
	{
//#ifdef _WIN32
//		//是否有数据到达
//		FD_ZERO(&tfdReadset);
//		FD_SET(sock.hSock, &tfdReadset);
//
//		int nRet = select((int)sock.hSock + 1, &tfdReadset, NULL, NULL, &tTimeout);
//		if (nRet <= 0)
//		{
//			if (!m_bTmOutCut && 0 == nRet)
//			{
//				continue;
//			}
//			Log("socket cut(nRet : %d, TmOut : %d) client ip is: %s\n", nRet, m_nSockSelecTmOut, m_strFromIP.c_str());
//			break;
//		}
//
//		if (!FD_ISSET(sock.hSock, &tfdReadset))
//		{
//			Msleep(10);
//			continue;
//		}
//#endif

		//开始接收数据，并对接收到的数据进行解析
		memset(pcRecvBuf, 0, sizeof(pcRecvBuf));
		if (sock.Recv(pcRecvBuf, MAX_RECV_BUF) <= 0)
		{
			Log("socket cut client ip is: %s\n", m_strFromIP.c_str());
			break;
		}

		strContentBuffer += pcRecvBuf;

		while(ParseHttpMsg(strContentBuffer, strMsg))
		{
			//写交换文件
			//printf("ParseHttpMsg: %s\n", strMsg.c_str());

			if (m_bStop)
			{
				break;
			}

			//begin检查重复数据
			//解析MSGID
			std::string strMsgID = "200";
			//ParseMsgID(strMsg, strMsgID);

			std::string strReply = "HTTP/1.0 " + strMsgID + " OK\r\n\r\n";					

			//Log("**************RECEV HTTP FROM IP: %s", m_strFromIP.c_str());
			//立即发送HTTP + MsgID + OK数据
			int nSendLen = sock.Send(strReply.data(), (int)strReply.size());
			if (nSendLen != (int)strReply.size())
			{
				Log("Send Msg Reply Err:\n %s",strReply.c_str());
				continue;
			}

			//检查是否存在已经处理的重复命令
			int nMsgID = atoi(strMsgID.c_str());

			int nBufIndex = nMsgID & REAPET_CHECK_NUM;
			if (m_pBufMsgID[nBufIndex] != nMsgID)
			{
				m_pBufMsgID[nBufIndex] = nMsgID;
			}
			else
			{
				Log("****Recv Reapet Msg, ID is: %d\n", nMsgID);
			}
			//end检查重复数据


			//解析Code
			if (!m_bParseObjEffect)
			{
				//ParseSrcCode(strMsg, strSrcCode);
				//m_strFromCode = strSrcCode;
				m_strFromCode = "this";

				if (m_lpContext && m_pFnGetXMLParser)
				{
					CXMLParser* pObj = NULL;
					m_pFnGetXMLParser(m_lpContext, m_strFromCode, pObj);
					if (pObj)
					{
						m_pParseObject = pObj;
						m_bParseObjEffect = true;
						printf("****  HTTP get one effect conection\n");
					}
					else
					{
						Log("Http module can't find CXMLParser object instance, src code is: %s", m_strFromCode.c_str());
						// ? 打印日志
					}
				}

				if (!m_bParseObjEffect)
				{
					Msleep(100);
					continue;
				}
			}
			
			//通知调用者来取数据
			if (m_bParseObjEffect && m_pParseObject)
			{
				m_pParseObject->ParseXML(strMsg);
			}
			else
			{
				Log("HTTP DATA CAN'T FIND RECVER: FROM IP: %s", m_strFromIP.c_str());
			}
		}
	}
}

bool CHttpRecvTH::ParseSrcCode(const std::string& strMsg, std::string& strSrcCode)
{
	CMarkup xml;
	xml.SetDoc(strMsg.c_str());
	if (!xml.FindElem("Msg"))
	{
		return false;
	}

	strSrcCode = xml.GetAttrib("SrcCode");

	return true;
}

bool CHttpRecvTH::ParseDstCode(const std::string& strMsg, std::string& strSrcCode)
{
	CMarkup xml;
	xml.SetDoc(strMsg.c_str());
	if (!xml.FindElem("Msg"))
	{
		return false;
	}

	strSrcCode = xml.GetAttrib("DstCode");

	return true;
}

bool CHttpRecvTH::ParseMsgID(const std::string& strMsg, std::string& strMsgID)
{
	strMsgID = "err";

	CMarkup xml;
	xml.SetDoc(strMsg.c_str());
	if (!xml.FindElem("Msg"))
	{
		return false;
	}

	strMsgID = xml.GetAttrib("MsgID");

	return true;
}

void CHttpRecvTH::CreateReplyXml(const std::string& strMsgID, std::string& strMsg)
{
	strMsg = "<?xml version=\"1.0\" encoding=\"GB2312\" standalone=\"yes\" ?>\n";
	strMsg += "<Msg>\n";
	strMsg += "<Return MsgID=\"";
	strMsg += strMsgID;
	strMsg += "\"/>" ;
	strMsg += "</Msg>\n";
}

/*
* Lower-cases all the characters in the std::string.
*/
static void toLowerCase(std::string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

bool CHttpRecvTH::ParseHttpMsg(std::string& strMsg, std::string& strHttp)
{
	//转换为小写处理
	std::string strMsgTemp = strMsg;
	toLowerCase(strMsgTemp);

	//读取HTTP有效数据长度 
	size_t nPosLengthBegin, nPosLengthEnd;
	if ((nPosLengthBegin = strMsgTemp.find(pcContentLength)) == std::string::npos)
	{
		return false;
	}
	if ((nPosLengthEnd = strMsgTemp.find("\r\n", nPosLengthBegin)) == std::string::npos)
	{
		return false;
	}

	nPosLengthBegin += strlen(pcContentLength);
	nPosLengthBegin += 1;// + ":"

	std::string strContentLen = strMsgTemp.substr(nPosLengthBegin, nPosLengthEnd - nPosLengthBegin);
	int nContentLen = atoi(strContentLen.c_str());

	//读取有效内容
	//查找内容开始位置
	size_t nPosContentBegin;
	if ((nPosContentBegin = strMsgTemp.find("\r\n\r\n")) == std::string::npos)
	{
		return false;
	}
	nPosContentBegin += strlen("\r\n\r\n");
	if ((int)(nPosContentBegin + 1) > (int)strMsg.length())
	{
		return false;
	}

	//计算内容结束位置
	size_t nPosContentEnd = nPosContentBegin + nContentLen;
	if ((int)nPosContentEnd > (int)strMsg.length())
	{
		return false;
	}	
	else
	{
		strHttp = strMsg.substr(nPosContentBegin, nContentLen);
		if ((int)nPosContentEnd == (int)strMsg.length())
		{
			strMsg = "";
		}
		else
		{
			strMsg = strMsg.substr(nPosContentEnd);
		}		
	}

	return true;
}

//////////CHttpSendTH
BufIndex g_objBufIndex;
BufIndex::BufIndex()
{
	m_mutexBufIndex.Init();
	m_unBufIndex = 1;
}

BufIndex::~BufIndex()
{}

unsigned int BufIndex::Get()
{
	unsigned int unBufIndex = 0;
	m_mutexBufIndex.Lock();
	unBufIndex = ++m_unBufIndex;
	m_mutexBufIndex.Unlock();

	return unBufIndex;
}

CHttpSendTH::CHttpSendTH()
{
	m_pfnSendStatus = NULL;
	m_lpContext = NULL;
	m_bStop = false;
	m_bExitFlag = true;
	m_bSockConn = false;
	m_bLongConn = true;

	m_nSendTimeout = 10;

	m_strToIP = "";
	m_nToPort = 8095;

	CRingBuffer<std::string, 1000>* pBuf = new CRingBuffer<std::string, 1000>;
	if (NULL == pBuf)
	{
		Log("CHttpSendTH():new CRingBuffer<std::string, 1000> err");
		return;
	}
	pBuf->SetBufferName("CHttpSendTH():m_mapIndexBuf");

	m_mapIndexBuf[0] = pBuf;
	m_unBufIndex = 0;

	m_pSendSwapFile = NULL;
	m_nLine = 0;
	m_strPath = "";
	SetTHNum(1);
}

CHttpSendTH::CHttpSendTH(const std::string& strToIP, int nToPort, bool bLong)
{
	if (strToIP.empty())
	{
		Log("CHttpSendTH:strToIP is empty");
		m_bExitFlag = false;
		return;
	}

	CRingBuffer<std::string, 1000>* pBuf = new CRingBuffer<std::string, 1000>;
	if (NULL == pBuf)
	{
		Log("CHttpSendTH():new CRingBuffer<std::string, 1000> err");
		return;
	}
	pBuf->SetBufferName("CHttpSendTH():m_mapIndexBuf");
	
	m_mapIndexBuf[0] = pBuf;
	m_unBufIndex = 0;

	m_strToIP = strToIP;
	m_nToPort = nToPort;
	m_bLongConn = bLong;

	m_nSendTimeout = 10;

	m_bStop = false;
	m_bExitFlag = false;
	m_bSockConn = false;

	m_pSendSwapFile = NULL;
	m_nLine = 0;
	m_strPath = "";
	SetTHNum(1);
}

CHttpSendTH::~CHttpSendTH()
{
	m_pfnSendStatus = NULL;
	m_lpContext = NULL;

	Stop();

	IndexBufMap::iterator it = m_mapIndexBuf.begin();
	for (; it != m_mapIndexBuf.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}

	if (m_pSendSwapFile)
	{
		fclose(m_pSendSwapFile);
		m_pSendSwapFile = NULL;
	}
}

void CHttpSendTH::SetPath(std::string &strPath)
{
	m_strPath = strPath;
}


void CHttpSendTH::RegCallBackFunc(
					 HttpSendCallBack pfnHttpSend,
					 void *lpContext
					 )
{
	m_pfnSendStatus = pfnHttpSend;
	m_lpContext = lpContext;
}

int CHttpSendTH::Start()
{
	m_bStop = false;
	m_bExitFlag = false;

	if(0 != Run())
	{
		m_bExitFlag = true;
		m_bStop = true;
		Log("CHttpRecvTH::Start() Run() < 0");
		return -1;
	}

	return 0;
}

void CHttpSendTH::Stop()
{
	m_bStop = true;
	OS_Thread::Join(this);
}

void CHttpSendTH::SetExit()
{
	m_bStop = true;
}

bool CHttpSendTH::IsExit()
{ 
	return m_bExitFlag; 
}

void CHttpSendTH::SetSendTimeout(int nTimeout) 
{ 
	m_nSendTimeout = nTimeout; 
}

bool CHttpSendTH::IsSame(const std::string& strToIP, int nToPort)
{
	if (strToIP == m_strToIP && nToPort == m_nToPort)
	{
		return true;
	}

	return false;
}

void CHttpSendTH::RealeseBuf(unsigned int unBufIndex)
{
	m_bufNeedDel.Push(unBufIndex);
}

void CHttpSendTH::AddMsg(const std::string& strMsg, unsigned int nBufIndex)
{
	IndexBufMap::iterator it = m_mapIndexBuf.find(nBufIndex);
	if (it != m_mapIndexBuf.end())
	{
		if (NULL != it->second)
		{
			it->second->Push(strMsg);
			return;
		}
	}
	else
	{
		CRingBuffer<std::string, 1000>* pBuf = NULL;	

		pBuf = new CRingBuffer<std::string, 1000>;
		if (NULL == pBuf)
		{
			Log("CHttpSendTH()::AddMsg :new CRingBuffer<std::string, 1000> err");
			return;
		}
		pBuf->SetBufferName("CHttpSendTH():m_mapIndexBuf, size:1000");
		m_mapIndexBuf[nBufIndex] = pBuf;

		pBuf->Push(strMsg);

		return;
	}

	Log("CHttpSendTH::AddMsg(): err");
}

void CHttpSendTH::AddMsg(const MsgVec& vecMsg)
{
	bool bFind = false;
	IndexBufMap::iterator it = m_mapIndexBuf.find(0);
	if (it != m_mapIndexBuf.end())
	{
		if (NULL != it->second)
		{
			bFind = true;
		}
	}

	if (bFind)
	{
		for (size_t i = 0; i < vecMsg.size(); ++i)
		{
			it->second->Push(vecMsg[i]);
		}
	}
}

int CHttpSendTH::Routine(TH1)
{
	bool bDo = false;
	std::string strMsg("");
	CRingBuffer<std::string, 1000>* pBuf = NULL;
	IndexBufMap::iterator it;

	while (!m_bStop)
	{
		bDo = false;
		int nStatus = 0;

		//发送数据
		if (m_bLongConn)//长连接
		{
			it = m_mapIndexBuf.begin();
			for (; it != m_mapIndexBuf.end(); ++it)
			{
				if (m_bStop)
				{
					break;
				}

				pBuf = it->second;
				if (pBuf != NULL)
				{
					while (!pBuf->IsEmpty())
					{
						if (m_bStop)
						{
							break;
						}

						strMsg = pBuf->Pop();

						nStatus = (0 != PostMsg(strMsg)) ? 0 : 1;

						if (m_pfnSendStatus && m_lpContext && m_strToIP != "127.0.0.1")
						{
							m_pfnSendStatus(m_strToIP, m_nToPort, nStatus, m_lpContext);
						}

						bDo = true;
					}
				}
			}
		}
		else//短连接
		{
			it = m_mapIndexBuf.find(0);
			if (it != m_mapIndexBuf.end())
			{
				pBuf = it->second;
				if (pBuf != NULL)
				{
					while (!pBuf->IsEmpty())
					{
						if (m_bStop)
						{
							break;
						}

						strMsg = pBuf->Pop();

						PostMsg(strMsg);

						bDo = true;
					}
				}
			}

			if (bDo)
			{
				break;
			}
		}

		//释放无用BUF
		unsigned int unIndex = 0;
		while (!m_bufNeedDel.IsEmpty())
		{
			unIndex = m_bufNeedDel.Pop();

			IndexBufMap::iterator itDel = m_mapIndexBuf.find(unIndex);
			if (itDel!= m_mapIndexBuf.end())
			{
				SAFE_DELETE(itDel->second);
				m_mapIndexBuf.erase(itDel);
			}
		}

		if (!bDo)
		{
			Msleep(10);
		}
	}

	m_bExitFlag = true;

	m_SendSock.Close();
	m_bSockConn = false;

	return 0;
}

int CHttpSendTH::PostMsg(const std::string& strMsg)
{
	for (int i = 0; i < 2; ++i)
	{
		m_bSockConn ? NULL : m_bSockConn = Connect();

		if (!m_bSockConn)
		{
			Log("CHttpSendTH::PostMsg(): connect socket err for SendAddr: %s:%d!", 
				m_strToIP.c_str(), m_nToPort);

			return -1;
		}

		if (Send(strMsg))
		{
			//print("HttpSendTH PostMsg:%s\n", strMsg.c_str());
			return 0;
		}
		else
		{
			m_bSockConn = false;
			m_SendSock.Close();

			return -2;	
		}
	}

	Log("CHttpSendTH::PostMsg(): Send(strMsg) err for SendAddr:: %s:%d!", 
		m_strToIP.c_str(), m_nToPort);

	return -2;
}

bool CHttpSendTH::Connect()
{
	//打开Sock
	if (m_SendSock.Open() < 0)
	{
		Log("CHttpSendTH::Connect() Open socket err for SendAddr: %s:%d!", 
			m_strToIP.c_str(), m_nToPort);
		return false;
	}

	//先设置为非阻塞模式，连接超时返回
	if (m_SendSock.Ioctl_SetBlockedIo(false) < 0)
	{
		Log("CHttpSendTH::Connect() Ioctl_SetBlockedIo() err for SendAddr: %s:%d!", 
			m_strToIP.c_str(), m_nToPort);
		return false;
	}

	//连接到目标地址
	if (m_SendSock.Connect(OS_SockAddr(m_strToIP.c_str(), m_nToPort)) < 0)
	{
		//判断有无数据发送过来
		fd_set tfdWrset;
		FD_ZERO(&tfdWrset);
		FD_SET(m_SendSock.hSock, &tfdWrset);
		struct timeval tTimeout;	//1秒超时
		tTimeout.tv_sec = 1;
		tTimeout.tv_usec = 0;
		int iRet = select((int)m_SendSock.hSock + 1, NULL, &tfdWrset, NULL, &tTimeout);

		if ((!FD_ISSET(m_SendSock.hSock, &tfdWrset))||(1 > iRet))
		{
			//关闭连接
			m_SendSock.Close();

			Log("CHttpSendTH::Connect() fdWrset err for SendAddr: %s:%d!", 
				m_strToIP.c_str(), m_nToPort);

			return false;
		}
	}

	//设置回阻塞模式
	if (m_SendSock.Ioctl_SetBlockedIo(true) < 0)
	{
		return false;
	}

	m_SendSock.SetOpt_SendTimeout(10000);
	m_SendSock.SetOpt_RecvTimeout(10000);

	//Log("CHttpSendTH::Connect() successful for SendAddr:: %s:%d!", 
	//	m_strToIP.c_str(), m_nToPort);

	return true;
}

bool CHttpSendTH::Send(const std::string& strMsg)
{
	//封装为HTTP数据
 	std::string strHttpMsg = strMsg;
	StuffMsg(strHttpMsg, m_strToIP, m_nToPort);

	//发送数据
	int nBytesSend = m_SendSock.Send(strHttpMsg.data(), (int)strHttpMsg.size());

	bool bContinue = (nBytesSend < (int)strHttpMsg.size()) ? false : true;
	if (! bContinue)
	{
		return false;
	}

	//判断有无数据发送过来
	fd_set tfdReadset;
	FD_ZERO(&tfdReadset);
	FD_SET(m_SendSock.hSock, &tfdReadset);
	struct timeval tTimeout;
	tTimeout.tv_sec = m_nSendTimeout;
	tTimeout.tv_usec = 0;
	int nRet = select((int)m_SendSock.hSock + 1, &tfdReadset, NULL, NULL, &tTimeout);
	if (0 < nRet)
	{
		if (!FD_ISSET(m_SendSock.hSock, &tfdReadset))
		{
			m_SendSock.Close();				
			return false;
		}
	}
	else if (0 == nRet) // 超时
	{
		return false;
	}
	else // 错误
	{
		m_SendSock.Close();
		return false;
	}

	char buff[1024] = {0};
	int nCount = m_SendSock.Recv(buff, 1024);
	if (nCount <= 0)
	{
		m_SendSock.Close();
 		return false;
	}

	// 判断数据是否返回成功
	std::string strReply = buff;

	if (CheckReply(strReply))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CHttpSendTH::CheckReply(const std::string& strReply)
{
	if (strReply.find("200 OK") >=0)
	{
		return true;
	}

	return false;
}

bool CHttpSendTH::StuffMsg(std::string &strContent, const std::string& strToIP, int nToPort)
{
	std::string strHttpHeader;
	strHttpHeader += ("POST ");
	strHttpHeader += m_strPath;
	strHttpHeader += " ";

	std::string strPost;

	//继续查找:后的端口号
	strHttpHeader += ("HTTP/1.1\r\n");
	strHttpHeader += ("Accept: */*\r\n");
	strHttpHeader += ("Accept-Language: zh-cn\r\n");
	strHttpHeader += ("Accept-Encoding: gzip, deflate\r\n");
	strHttpHeader += ("User-Agent: Mozilla/4.0 (compatible; MSIE 5.5; Windows 98)\r\n");

	char szTemp[256] = {0};
	snprintf(szTemp, 256, "Host: %s:%d\r\n", strToIP.c_str(), nToPort);
	strHttpHeader.append(szTemp, strlen(szTemp));

	snprintf(szTemp, 256, "Content-Length: %d\r\n", (int)(strContent.size()));
	strHttpHeader.append(szTemp, strlen(szTemp));

	strHttpHeader += "\r\n";

	// 插入到要发送的内容前面
	strContent.insert(0, strHttpHeader);

	return true;
}


//////////CHttpStation
CHttpStation::CHttpStation()
{
	m_strIP = "127.0.0.1";
	m_nPort = 0;
	m_bStop = true;

	m_mutexRecvTH.Init();
	m_mutexSendTH.Init();
	m_mutexXMLParserMap.Init();
	SetTHNum(2);
}

CHttpStation::~CHttpStation()
{
	Stop();
	Realese();
}

void CHttpStation::SetLocalAddr(const std::string strLocalIP, int nLocalPort)
{
	m_strIP = strLocalIP.c_str();	
	m_nPort = nLocalPort;
}

bool CHttpStation::Start(void)
{
	// 先判断一下能否在指定的地址和端口上侦听
	OS_SockAddr addrLocal = OS_SockAddr(m_strIP.c_str(), m_nPort);
	if(m_listenSock.Open(addrLocal, true) < 0)
	{	
		log_info("m_listenSock.Open err, addr:%s:%d", m_strIP.c_str(), m_nPort);
		return false;
	}

	printf("http listen: local ip(%s), port(%d)\n", m_strIP.c_str(), m_nPort);

	//监听
	if(m_listenSock.Listen(20) < 0)
	{
		return false;
	}

	m_bStop = false;
	if (0 != Run())
	{
		m_bStop = true;

		return false;
	}

	return true;
}

//终止线程
void CHttpStation::Stop(void)
{
	if (!m_bStop)
	{
		m_bStop = true;
		OS_Thread::Join(this);
	}
	
	Realese();
}

void CHttpStation::SetExit()
{
	m_bStop = true; 
}

bool CHttpStation::RegParseXmlObj(const std::string& strCode, CXMLParser* pObj)
{
	m_mutexXMLParserMap.Lock();
	if (m_mapXMLParser.find(strCode) == m_mapXMLParser.end())
	{
		m_mapXMLParser[strCode] = pObj;
	}
	else
	{
		m_mutexXMLParserMap.Unlock();
		return false;
	}
	m_mutexXMLParserMap.Unlock();

	return true;
}

void CHttpStation::Realese()
{
	//设置接收和发送线程退出信号
	m_mutexRecvTH.Lock();
	for (unsigned int i=0; i<m_vecPHttpRecvTH.size(); ++i)
	{
		if (m_vecPHttpRecvTH[i] != NULL)
		{
			m_vecPHttpRecvTH[i]->SetExit();
		}
	}
	m_mutexRecvTH.Unlock();

	m_mutexSendTH.Lock();
	for (unsigned int i=0; i<m_vecPHttpSendTH.size(); ++i)
	{
		if (m_vecPHttpSendTH[i] != NULL)
		{
			m_vecPHttpSendTH[i]->SetExit();
		}
	}
	m_mutexSendTH.Unlock();


	//销毁资源
	m_mutexRecvTH.Lock();
	for (unsigned int i=0; i<m_vecPHttpRecvTH.size(); ++i)
	{
		SAFE_DELETE(m_vecPHttpRecvTH[i]);
	}
	m_vecPHttpRecvTH.clear();
	m_mutexRecvTH.Unlock();

	m_mutexSendTH.Lock();
	for (unsigned int i=0; i<m_vecPHttpSendTH.size(); ++i)
	{
		SAFE_DELETE(m_vecPHttpSendTH[i]);
	}
	m_vecPHttpSendTH.clear();
	m_mutexSendTH.Unlock();
}

//处理监听
int CHttpStation::Routine(TH1)
{
	CHttpRecvTH* pHttpRecvTH = NULL;
	std::string strFromIP("");
	int nFromPort = 0;
	std::string strFromAddr("");
	OS_SockAddr sockAddr;

	fd_set tfdReadset;
	struct timeval tTimeout;

	while (!m_bStop)
	{
		tTimeout.tv_sec = 0;
		tTimeout.tv_usec = 10000;

		Msleep(10);

		//检查是否有数据到达
		FD_ZERO(&tfdReadset);
		FD_SET(m_listenSock.hSock, &tfdReadset);
		
		if (-1 == select((int)m_listenSock.hSock + 1, &tfdReadset, NULL, NULL, &tTimeout))
		{
			continue;
		}

		if (!FD_ISSET(m_listenSock.hSock, &tfdReadset))
		{
			continue;
		}

		OS_TcpSocket sock;
		
		if (0 == m_listenSock.Accept(&sock))
		{
			pHttpRecvTH = new CHttpRecvTH(sock);
			pHttpRecvTH->RegFun(this, GetXMLParser);

			sock.GetPeerAddr(sockAddr);
			strFromIP = sockAddr.GetIp_str();
			nFromPort = sockAddr.GetPort();
			CheckRepeatRecvTH(strFromIP, nFromPort);

			if (pHttpRecvTH)
			{
				//添加到接收线程容器中
				m_mutexRecvTH.Lock();
				if (m_vecPHttpRecvTH.size() >= MAX_RECV_TH_NUM)
				{
					printf("****HTTP Module find the recv instance'num is too big, the Max num is: %d\n", MAX_RECV_TH_NUM);
					SAFE_DELETE(pHttpRecvTH);
				}
				else
				{
					pHttpRecvTH->Start();
					printf("**** One Http Recv Therad is Create\n");
					m_vecPHttpRecvTH.push_back(pHttpRecvTH);
				}
				m_mutexRecvTH.Unlock();
			}
			else
			{
				Log("CHttpStation::Routine: new pHttpRecvTH err");
			}
		}
	}

	// 关闭侦听
	m_listenSock.Close();

	return 0;
}

void CHttpStation::GetTheXMLParser(const string& strCode, CXMLParser*& pObj)
{
	m_mutexXMLParserMap.Lock();
	XMLParserMap::iterator it = m_mapXMLParser.find(strCode);
	if (it != m_mapXMLParser.end())
	{
		pObj = it->second;
	}
	else
	{
		pObj = NULL;
	}
	m_mutexXMLParserMap.Unlock();
}

void CHttpStation::GetXMLParser(void* lp, const string& strCode, CXMLParser*& pObj)
{
	CHttpStation* pThis = (CHttpStation*)lp;
	if (!pThis)
	{
		pObj = NULL;
		return;
	}

	pThis->GetTheXMLParser(strCode, pObj);
}

void CHttpStation::CheckRepeatRecvTH(const std::string& strClientIP, int nClientPort)
{
	CHttpRecvTH* pRecvTH = NULL;

	m_mutexRecvTH.Lock();
	PHttpRecvTHVec::iterator it = m_vecPHttpRecvTH.begin();
	for (; it!=m_vecPHttpRecvTH.end(); ++it)
	{
		pRecvTH = (CHttpRecvTH*)(*it);
		if (pRecvTH != NULL)
		{
			if (pRecvTH->IsSame(strClientIP, nClientPort))
			{
				pRecvTH->SetExit();
				for (int i=0; i<500; ++i)
				{
					Msleep(10);
					if (pRecvTH->IsExit())
					{
						break;
					}
				}
				SAFE_DELETE(pRecvTH);

				m_vecPHttpRecvTH.erase(it);
				Log("One Http Recv Thread is Exited");
				break;
			}
		}
	}
	m_mutexRecvTH.Unlock();
}

//回收资源线程
int CHttpStation::Routine(TH2)
{
	CHttpRecvTH* pRecvTH = NULL;
	CHttpSendTH* pSendTH = NULL;

	int nTmCount = 0;
	while(!m_bStop)
	{
		//1秒清理一次
		Msleep(10);
		nTmCount += 10;
		if (nTmCount < 1000)
		{
			continue;
		}
		nTmCount = 0;

		//检查接收线程
		m_mutexRecvTH.Lock();
		PHttpRecvTHVec::iterator itRecv = m_vecPHttpRecvTH.begin();
		for (; itRecv != m_vecPHttpRecvTH.end();)
		{
			pRecvTH = (CHttpRecvTH*)(*itRecv);
			if (pRecvTH != NULL)
			{
				if (pRecvTH->IsExit())
				{
					SAFE_DELETE(pRecvTH);
					itRecv = m_vecPHttpRecvTH.erase(itRecv);
					continue;
				}
			}
			++ itRecv;
		}
		m_mutexRecvTH.Unlock();

		//检查发送线程
		m_mutexSendTH.Lock();
		PHttpSendTHVec::iterator itSend = m_vecPHttpSendTH.begin();
		for (; itSend !=m_vecPHttpSendTH.end();)
		{
			pSendTH = (CHttpSendTH*)(*itSend);
			if (pSendTH != NULL)
			{
				if (pSendTH->IsExit())
				{
					SAFE_DELETE(pSendTH);
					itSend = m_vecPHttpSendTH.erase(itSend);
					continue;
				}
			}
			++ itSend;
		}
		m_mutexSendTH.Unlock();
	}

	return 0;
}

//发送相关
CHttpSendTH* CHttpStation::CreateSender(const std::string& strToIP, int nToPort, bool bLongConn)
{
	CHttpSendTH* objHttpSendTH = new CHttpSendTH(strToIP, nToPort, bLongConn);

	if (NULL == objHttpSendTH)
	{
		Log("CHttpStation::AddSender():new CHttpSendTH err for SendAddr is:%s:%d",
			strToIP.c_str(), nToPort);

		return NULL;
	}

	if (0 != objHttpSendTH->Start())
	{
		Log("CHttpStation::AddSender():Start err for SendAddr is:%s:%d",
			strToIP.c_str(), nToPort);

		return NULL;
	}

	m_mutexSendTH.Lock();
	m_vecPHttpSendTH.push_back(objHttpSendTH);
	m_mutexSendTH.Unlock();

	return objHttpSendTH;
}

//长连接
bool CHttpStation::PostMsg(const std::string& strMsg, void* lpSend, unsigned int nBufIndex)
{
	if (m_bStop)
	{
		return false;
	}

	CHttpSendTH* pSender = NULL;
	if (!lpSend)
	{
		return false;
	}

	pSender = (CHttpSendTH*)lpSend;
	if (!pSender)
	{
		return false;
	}

	pSender->AddMsg(strMsg, nBufIndex);

	return true;
}

//短连接
bool CHttpStation::PostMsg(const MsgVec& vecMsg, const std::string& strToIP, int nToPort, string strPath)
{
	if (m_bStop)
	{
		return false;
	}

	CHttpSendTH* pSender = CreateSender(strToIP, nToPort, false);
	if (!pSender)
	{
		return false;
	}

	pSender->SetPath(strPath);
	pSender->AddMsg(vecMsg);

	return true;
}

void CHttpStation::RealeseSendBuf(void* lpSend, unsigned int nBufIndex)
{
	CHttpSendTH* pSend = (CHttpSendTH*)lpSend;
	if (pSend)
	{
		pSend->RealeseBuf(nBufIndex);
	}
}

void CHttpStation::DelRecvTH(const std::string& strFromIP, int nFromPort)
{
	CHttpRecvTH* pRecvTH = NULL;

	m_mutexRecvTH.Lock();
	PHttpRecvTHVec::iterator itRecv = m_vecPHttpRecvTH.begin();
	for (; itRecv != m_vecPHttpRecvTH.end(); ++itRecv)
	{
		pRecvTH = (CHttpRecvTH*)(*itRecv);
		if (pRecvTH != NULL)
		{
			if (pRecvTH->IsSame(strFromIP, nFromPort))
			{
				SAFE_DELETE(pRecvTH);
				m_vecPHttpRecvTH.erase(itRecv);
				break;
			}
		}
	}
	m_mutexRecvTH.Unlock();
}

void CHttpStation::DelRecvTH(const std::string& strFromCode)
{
	CHttpRecvTH* pRecvTH = NULL;

	//删除对应的XML解析器
	m_mutexXMLParserMap.Lock();
	XMLParserMap::iterator it = m_mapXMLParser.find(strFromCode);
	if (it != m_mapXMLParser.end())
	{
		m_mapXMLParser.erase(it);
	}
	m_mutexXMLParserMap.Unlock();


	m_mutexRecvTH.Lock();
	PHttpRecvTHVec::iterator itRecv = m_vecPHttpRecvTH.begin();
	for (; itRecv != m_vecPHttpRecvTH.end(); ++itRecv)
	{
		pRecvTH = (CHttpRecvTH*)(*itRecv);
		if (pRecvTH != NULL)
		{
			if (pRecvTH->IsSame(strFromCode))
			{
				SAFE_DELETE(pRecvTH);
				m_vecPHttpRecvTH.erase(itRecv);
				break;
			}
		}
	}
	m_mutexRecvTH.Unlock();
}

void CHttpStation::DelSendTH(const std::string& strToIP, int nToPort)
{
	CHttpSendTH* pSendTH = NULL;
	
	m_mutexSendTH.Lock();
	PHttpSendTHVec::iterator itSend = m_vecPHttpSendTH.begin();
	for (; itSend !=m_vecPHttpSendTH.end(); ++ itSend)
	{
		pSendTH = (CHttpSendTH*)(*itSend);
		if (pSendTH != NULL)
		{
			if (pSendTH->IsSame(strToIP, nToPort))
			{
				SAFE_DELETE(pSendTH);
				m_vecPHttpSendTH.erase(itSend);
				break;
			}
		}
	}
	m_mutexSendTH.Unlock();
}
};
