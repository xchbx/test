#include <sys/reboot.h>
#include <sys/socket.h>
#include "PlatformInterface.h"
#include "FileEx.h"
#include "NetcardInfo.h"
#include "ChannelManager.h"

using namespace Tool;

#include <iconv.h>


#define SYS_REBOOT_LOG   "./reboot.log"
#define SOCK_DATA_LEN		1024

// 主动发送组播协议 相关宏定义 Start
#define BOARD_TYPE 			1	// 转码卡

#define BOARD_CHASSIS       0   // 机框号
#define BOARD_SLOT          15  // 卡槽号

#define MAX_BUFFER_LEN      32
#define HEAD_SYNC			(0x5a)
#define MSG_HEAD_LEN		4
#define LOAD_HEAD_LEN		4
#define MSG_CRC_LEN			4

#define LOAD_TYPE_OFFSET 	MSG_HEAD_LEN
#define LOAD_LEN_OFFSET	    (LOAD_TYPE_OFFSET + 1)
#define LOAD_DATA_OFFSET	(LOAD_TYPE_OFFSET + 4)    

#define BOARD_CHECK_DATA_LEN	12      // 板卡自检 负载长度(Byte)
#define CH_RESOURCE_DATA_LEN	16      // 板卡资源 负载长度(Byte)
#define AV_ERR_STATUS_DATA_LEN	8       // 通道报警 负载长度(Byte)

#define BOARD_CHECK_MSG_LEN	    (MSG_HEAD_LEN + LOAD_HEAD_LEN + BOARD_CHECK_DATA_LEN + MSG_CRC_LEN)
#define CH_RESOURCE_MSG_LEN	    (MSG_HEAD_LEN + LOAD_HEAD_LEN + CH_RESOURCE_DATA_LEN + MSG_CRC_LEN)
#define AV_ERR_STATUS_MSG_LEN	(MSG_HEAD_LEN + LOAD_HEAD_LEN + AV_ERR_STATUS_DATA_LEN + MSG_CRC_LEN)
// 主动发送组播协议 相关宏定义 End

#define   SNTP_DISABLE   (0)
#define   SNTP_ENABLE    (1)

#define CHINA_TIME_FIELD	28800			//北京时间 +8
#define INIT_TIME			0x83AA7E80		//3600*24*25567//1970年1月1日相对于1900年1月1日之间的秒数

#define  SH_FILE_NAME       "/tmp/upgrade.sh"
#define  TAR_FILE_NAME      "/tmp/tmpFile.tar.gz"
#define  SERPORT		     6666
#define  OUT_TIME_MAX        12

extern const char * g_strHardwareVersion;
extern const char * g_strSoftwareVersion;
static PlatformInterface *g_pPlatformInterface = NULL;


static bool ParseAddrToIPPort(string strAddr, string &strIP, int &nPort);
static void Reboot();
static char *ReadFile(const char *cstrPath);
static bool WriteFile(const char *cstrPath, const char *cstrContext);
static char *helper_iconv(const char *encFrom, const char *encTo, char *in);


PlatformInterface::PlatformInterface(void)
: m_bRun(false)
, m_hWorkThread(Tool::CZThread::INVALID_HANDLE)
, m_SocketFd (-1)
, m_SNTPPort (123)
, m_uTimeout10S_Count (0)
, m_uUpdateTime (0)
, m_uGap (60*10)
{
    m_mutexRingBuf.Init();

    InitCRC();
    RegParseInterface();
}

PlatformInterface::~PlatformInterface(void)
{
	Stop();
}

PlatformInterface* PlatformInterface::GetInstance() 
{
    if (g_pPlatformInterface == NULL)
    {
        g_pPlatformInterface = new PlatformInterface();
    }

    return g_pPlatformInterface;
}

void PlatformInterface::DelInstance()
{
    if (g_pPlatformInterface != NULL)
    {
        SAFE_DELETE(g_pPlatformInterface);
    }
}

bool PlatformInterface::Start()
{
	if (m_bRun) return true;

    SysConfig &stSysCfg = CConfig::GetInstance()->GetConfig();
    m_objHttpStation.SetLocalAddr(stSysCfg.strCtrlNetIP, stSysCfg.nCtrlPort);
    m_objHttpStation.RegParseXmlObj("this", this);

    if (!m_objHttpStation.Start())
    {
        printf("PlatformInterface::Start HTTP(%s:%d) module failed\n", stSysCfg.strCtrlNetIP.c_str(), stSysCfg.nCtrlPort);
        return false;
    }

	RestartReport();
    m_bRun = true;
	//启动与外部的通信线程
	m_hWorkThread = CZThread::BeginThread(TH_Work, this);

	//校时服务器线程
	m_hSntpThread = CZThread::BeginThread(TH_Sntp, this);

	return true;
}

void PlatformInterface::Stop()
{
    if(!m_bRun) return ;
	m_bRun = false;

	CZThread::WaitThreadEnd(m_hWorkThread);
	CZThread::WaitThreadEnd(m_hSntpThread);

    m_objHttpStation.Stop();
    m_sockMulitPost.Close();
}

void PlatformInterface::RegParseInterface()
{
    //数据处理接口映射集 HTTP接口
    m_mapDoCmdFn["GetDeviceInfo"] = &PlatformInterface::CmdGetDeviceInfo;
    m_mapDoCmdFn["SetOutputInfo"] = &PlatformInterface::CmdSetOutputInfo;
    m_mapDoCmdFn["GetOutputInfo"] = &PlatformInterface::CmdGetOutputInfo;
    m_mapDoCmdFn["Update"] = &PlatformInterface::CmdUpdate;
    m_mapDoCmdFn["Restart"] = &PlatformInterface::CmdRestart;
    m_mapDoCmdFn["Factory"] = &PlatformInterface::CmdFactory;
    m_mapDoCmdFn["SaveAllCfg"] = &PlatformInterface::CmdSaveAllCfg;
    m_mapDoCmdFn["SetTimeServerAddr"] = &PlatformInterface::CmdSetTimeServerAddr;
    m_mapDoCmdFn["SetTcorderInfo"] = &PlatformInterface::CmdSetTcorderInfo;
    m_mapDoCmdFn["GetTcorderInfo"] = &PlatformInterface::CmdGetTcorderInfo;
    m_mapDoCmdFn["SetOSDInfo"] = &PlatformInterface::CmdSetOSDInfo;
    m_mapDoCmdFn["GetOSDInfo"] = &PlatformInterface::CmdGetOSDInfo;
}

void PlatformInterface::ParseXML(const string& strXML)
{
	log_info("Recv XML:\n%s\n\n", strXML.c_str());
    /*
    <?xml version="1.0" encoding="gb2312"?>
    <Msg Version="1.0" MsgID="10000" SrcUrl="http://172.17.6.5:1000">
        <Type>SetCardInfo</Type>
        <Data>
            <!--指令具体数据-->
        </Data>
    </Msg>
    */
    if (strXML.empty()) return;

    rapidxml::xml_document<> doc;
    try {
        // parse会改变 szXml 中的内容
        doc.parse<0>((char *)strXML.c_str());
    }
    catch (...)
    {
        printf("----err xml----\n%s\n----------\n", strXML.c_str());
        return;
    }

    rapidxml::xml_node<> * pMsg = doc.first_node("Msg");
    if (NULL == pMsg) return;

    string strMsgID = (pMsg->first_attribute("MsgID"))->value();
    string strSrcUrl = (pMsg->first_attribute("SrcUrl"))->value();

    rapidxml::xml_node<char> * pType = pMsg->first_node("Type");
    string strMsgType = pType->value();

    rapidxml::xml_node<char> * pData = pMsg->first_node("Data");

    if (m_mapDoCmdFn.find(strMsgType) != m_mapDoCmdFn.end())
    {
        // 解析获取命令完成之后 数据发送的IP Port
        int nFindStart = strSrcUrl.find_last_of("/");		//	http://172.17.6.5:1000
        nFindStart += 1;
        int nFindEnd = strSrcUrl.find(":", nFindStart);

        string strSendtoIP = strSrcUrl.substr(nFindStart, nFindEnd - nFindStart);
        nFindEnd += 1;
        int nSendtoPort = atoi(strSrcUrl.substr(nFindEnd).c_str());

        pfnDoCmd pFn = m_mapDoCmdFn[strMsgType];
        (this->*pFn)(pData, strMsgID, strSendtoIP, nSendtoPort);
    }
    else
    {
        log_error("type %s not found", strMsgType.c_str());
        return;
    }
}

void PlatformInterface::AddMsgToPlatform(const char * strMsg, string &strPlatformIP, int nPlatformPort)
{
    m_mutexRingBuf.Lock();
    ToPlatformMsg stMsg(strMsg, strPlatformIP, nPlatformPort);
    m_ringToPlatform.Push(stMsg);
    m_mutexRingBuf.Unlock();
}

Tool::TH_RET PlatformInterface::TH_Work(LPVOID lpContext)
{
	if(lpContext == NULL)
		return NULL;
	PlatformInterface *pPlatformInterface = (PlatformInterface *)lpContext;
	pPlatformInterface->Work();

	return NULL;
}

void PlatformInterface::Work()
{
	SysConfig &stSysConfig = CConfig::GetInstance()->GetConfig();
    unsigned long long ullLastCalcTime = 0;
    unsigned long long ullLastPostMultiTime = 0;

	while (m_bRun)
	{
        unsigned long long ullCurTime = OS_SysTime_Ms();

        while (!m_ringToPlatform.IsEmpty())
        {
            ToPlatformMsg stMsg = m_ringToPlatform.Pop();

            HttpStation::MsgVec msgVec;
            msgVec.push_back(stMsg.strMsg);
            m_objHttpStation.PostMsg(msgVec, stMsg.strPlatformIP, stMsg.nPlatformPort);
        }

        // 500ms推送一次组播信息
        if (ullCurTime - ullLastPostMultiTime > 500)
        {
            ullLastPostMultiTime = ullCurTime;

            MultiBoardCheck(stSysConfig);
            MultiChannelResource(stSysConfig);
            MultiAVErrorStatus(stSysConfig);
        }

        // 1s计算一次带宽 CPU 内存信息
        if (ullCurTime - ullLastCalcTime > 1000)
        {
            ullLastCalcTime = ullCurTime;
            
            // 计算实时带宽
            for (auto it=stSysConfig.mapNetInfo.begin(); it!=stSysConfig.mapNetInfo.end(); ++it)
            {
                NetInfo &stNetInfo = it->second;
                unsigned long long ullRecv = 0, ullSend = 0;

                CNetcardInfo::GetInstance()->GetBandWidth(stNetInfo.strIP, ullRecv, ullSend);

                printf("@@@@@@@@@ [ETH(%s) IN %.2f Mbps OUT %.2f Mbps]\n", stNetInfo.strIP.c_str(), 
                    (float) (ullRecv * 8) / (float) 1000000, (float) (ullSend * 8) / (float) 1000000);
            }
            printf("\n");

            //计算 CPU 内存占用信息
            CConfig::GetInstance()->UpdateSystemInfo();		

            //实时读取网卡信息
            CNetcardInfo::GetInstance()->Init();
        }

		OS_Thread::Msleep(50);
	}
}

bool PlatformInterface::CmdGetDeviceInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    SysConfig &stSysCfg = CConfig::GetInstance()->GetConfig();

    CBHXml xml;
    XML_HEAD_SUCCESS("GetDeviceInfoR", strMsgID.c_str());

    xml.AddElem("Data");
    xml.IntoElem();
    xml.AddElem("OEM", stSysCfg.strOEM.c_str());
    string strTemp = to_string(TRANSCODE_CARD_TYPE);
    xml.AddElem("CardType", strTemp.c_str()); // 转码卡类型
    xml.AddElem("CardID", stSysCfg.strCardID.c_str());

    xml.AddElem("IPInfo");
    xml.IntoElem();
    const NetInfo &stNetInfo = stSysCfg.mapNetInfo.find(stSysCfg.strCtrlNet)->second;

    xml.AddElem("NICName", stNetInfo.strName.c_str());
    xml.AddElem("IP", stNetInfo.strIP.c_str());
    xml.AddElem("Netmask", stNetInfo.strNetmask.c_str());
    xml.AddElem("Gateway", stNetInfo.strGateway.c_str());
    xml.OutOfElem();

    xml.AddElem("VerInfo");
    xml.IntoElem();
    xml.AddElem("HardwareVer", g_strHardwareVersion);
    xml.AddElem("AppVer", g_strSoftwareVersion);
    xml.OutOfElem();

    xml.AddElem("Custom");
    xml.OutOfElem();
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdSetOutputInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;
    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
        return false;

    SysChannel &stSysChannel = CConfig::GetInstance()->GetChannel(nChannelID);

    rapidxml::xml_node<char> * pTSOut = pData->first_node("TSOut");
    if (pTSOut)
    {
        rapidxml::xml_node<char> * pAddr = pTSOut->first_node("Addr");
        if (pAddr) ParseAddrToIPPort(pAddr->value(), stSysChannel.stTSOutAddr.strIP, stSysChannel.stTSOutAddr.nPort);
        rapidxml::xml_node<char> * pSwitch = pTSOut->first_node("Switch");
        if (pSwitch) stSysChannel.stTSOutAddr.nSwitch = atoi(pSwitch->value());
    }

    rapidxml::xml_node<char> * pMonitor = pData->first_node("Monitor");
    if (pMonitor)
    {
        rapidxml::xml_node<char> * pAddr = pMonitor->first_node("Addr");
        if (pAddr) ParseAddrToIPPort(pAddr->value(), stSysChannel.stMonitorOutAddr.strIP, stSysChannel.stMonitorOutAddr.nPort);
        rapidxml::xml_node<char> * pSwitch = pMonitor->first_node("Switch");
        if (pSwitch) stSysChannel.stMonitorOutAddr.nSwitch = atoi(pSwitch->value());
    }


    // 2.处理命令
    ChannelManager::GetInstance()->SetChannelMultiAddr(nChannelID, stSysChannel.stTSOutAddr, stSysChannel.stMonitorOutAddr);

    CBHXml xml;
    XML_HEAD_SUCCESS("SetOutputInfoR", strMsgID.c_str());
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdGetOutputInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;
    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
        return false;

    // 2.处理命令
    TSOutAddr stTSOutAddr;
    MonitorOutAddr stMonitorOutAddr;
    ChannelManager::GetInstance()->GetChannelMultiAddr(nChannelID, stTSOutAddr, stMonitorOutAddr);

    CBHXml xml;
    XML_HEAD_SUCCESS("GetOutputInfoR", strMsgID.c_str());
    xml.AddElem("Data");
    xml.IntoElem();

    xml.AddElem("TSOut");
    xml.IntoElem();

    string strTemp = stTSOutAddr.strIP + ":" + to_string(stTSOutAddr.nPort);
    xml.AddElem("Addr", strTemp.c_str());

    strTemp = to_string(stTSOutAddr.nSwitch);
    xml.AddElem("Switch", strTemp.c_str());
    xml.OutOfElem();

    xml.AddElem("Monitor");
    xml.IntoElem();
    strTemp = stMonitorOutAddr.strIP + ":" + to_string(stMonitorOutAddr.nPort);
    xml.AddElem("Addr", strTemp.c_str());
    strTemp = to_string(stMonitorOutAddr.nSwitch);
    xml.AddElem("Switch", strTemp.c_str());
    xml.OutOfElem();

    xml.OutOfElem();    // out of data
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

void *PlatformInterface::UpdateCommunication()
{
	static char NU_SendBuf[1024];
	int sockfd = -1, ret = -1;
	char  IP_buf[32]={0};
	struct sockaddr_in seraddr = {0};
	unsigned int   time_num=0;
	struct sockaddr_in cliaddr = {0};
	int nfileLength = 0;                //  文件总长度
	int nfileSize = 0;                  //  实际接收的文件总长度
	int nsinglefileLength = 0;          //  拆包后的文件长度，单个包不超过1024字节
	FILE *pFile = NULL;
	struct timeval timeout= {5,0};

	while (1)
	{
		if(-1 != sockfd)
		{
			close(sockfd);
			sockfd = -1;
		}

		socklen_t len = sizeof(sockfd);
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if( sockfd < 0 )
		{
			printf("upgrade ser exe: cannot get socket\n");
			perror("upgrade ser exe");
			return NULL;
		}
		printf("#### <%s> socket creat success, sockfd=%d\n", __func__, sockfd); 

		int opt = 1;
		if (-1 == setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,(char *)&opt,sizeof(opt)))
		{
			perror("upgrade ser exe setsockopt fail:");
			close(sockfd);
			return NULL;
		}

		seraddr.sin_family = AF_INET;		
		seraddr.sin_port = htons(SERPORT);	 

		//此处获取配置文件中网卡IP
		SysConfig &m_stSysCfg = CConfig::GetInstance()->GetConfig();
//		strcpy(IP_buf, m_stSysCfg.strCtrlNetIP.c_str());

		seraddr.sin_addr.s_addr = inet_addr(m_stSysCfg.strCtrlNetIP.c_str());

		ret = bind(sockfd, (const struct sockaddr *)&seraddr, sizeof(seraddr));
		if(ret < 0)
		{
			printf("@@@@ <%s_%d>\n", __func__, __LINE__);
			perror("upgrade ser exe:");
			close(sockfd);
			return NULL;
		}
		printf("#### <%s> bind success\n", __func__);

		ret = listen(sockfd, 1);
		if( ret < 0 )
		{
			printf("upgrade ser exe: listen error\n");
			perror("upgrade ser exe:");
		}
		printf("#### <%s> linsten success\n", __func__);

		//处理函数
		while (1)
		{
			//struct sockaddr_in hostCur;

			fd_set fdRead;
			FD_SET(sockfd, &fdRead);
			timeout.tv_sec = 5;
			int iRet = select(sockfd+1, &fdRead, NULL, NULL, &timeout);//接收来自客服端的数据
			if (iRet <= 0)//错误或等待超时
			{
				time_num++;
				printf("#### <%s> select err, iRet=%d, time_num=%d\n", __func__, iRet, time_num);
				if(time_num>OUT_TIME_MAX)
				{ 
					printf("long wait , exit upgrade\n");
					close(sockfd);
					return NULL;
				}
				printf("select out time %d\n",time_num);	
				continue;
			}
			if (!FD_ISSET(sockfd, &fdRead))
			{
				continue;
			}

			//data exchange
			len = sizeof(cliaddr);
			int clifd = accept(sockfd,( struct sockaddr*)&cliaddr, &len);
			if (clifd < 0)
			{
				printf("[DEBUG]===>accept failed\n");
				usleep(50000);
				continue;
			}

			printf("upgrade ser exe: client connect\n");
			system("rm -rf "TAR_FILE_NAME);

			while (1)
			{
				char byMsg[2048];//单个包头5字节+最长1024个字节的内容
				int nPos = 0;
				int nNum = read(clifd, byMsg, 2048);
				if (nNum > 0)
				{
					//fileLength protocol for .hex download
					if ((NULL == pFile) && (nNum >= 9) && (*byMsg == 0x5B))//获取升级文件头：  格式  0X5B+文件长度四字节
					{
						nfileLength = byMsg[5];
						nfileLength = (nfileLength<<8) + (unsigned char)byMsg[6];
						nfileLength = (nfileLength<<8) + (unsigned char)byMsg[7];
						nfileLength = (nfileLength<<8) + (unsigned char)byMsg[8];
						/*获取包头中的文件长度数据*/
						printf("upgrade exe: update package Len is %d\n", nfileLength);

						/*接收到升级文件头，返回升级头回应 格式： 0X5C + 四字节0*/
						NU_SendBuf[0] = 0x5C;
						NU_SendBuf[1] = 0;
						NU_SendBuf[2] = 0;
						NU_SendBuf[3] = 0;
						NU_SendBuf[4] = 0;
						write(clifd, NU_SendBuf, 5);
						/*完成升级应答，打开文件准备写入升级数据*/
						pFile =  fopen(TAR_FILE_NAME, "wb");
						if (NULL == pFile)
						{
							printf("upgrade ser exe: fopen error\n");
							perror("upgrade  ser exe");
						}

						//nNum -= 5;//文件长度标识回退，丢弃升级头内容
						//nPos += 5;//待写入标识，实际的写入升级数据
						continue;
					}
					else if (NULL != pFile)//已经完成升级应答，可以写入升级数据
					{
						if((nNum >= 5) && (*byMsg == 0x5D))//获取单个包的长度：  格式  0X5D+包长度四字节
						{
							nsinglefileLength = byMsg[5];
							nsinglefileLength = (nsinglefileLength<<8) + (unsigned char)byMsg[6];
							nsinglefileLength = (nsinglefileLength<<8) + (unsigned char)byMsg[7];
							nsinglefileLength = (nsinglefileLength<<8) + (unsigned char)byMsg[8];
							/*获取包长度数据*/
							//printf("upgrade exe: update single_Len is %d\n", nsinglefileLength);
							printf("*");

							nNum -= 5;//文件长度标识回退，丢弃升级头内容
							nPos += 5;//待写入标识，实际的写入升级数据
							fwrite(byMsg + nPos, nNum, 1, pFile);
							nfileSize += nNum;//接收升级数据的大小，去掉包头

							/*接收数据，发送应答，应答格式为：0X5E+实际已经接受的升级数据文件的总长度*/
							NU_SendBuf[0] = 0x5E;
							NU_SendBuf[1] = 0x04;
							NU_SendBuf[2] = 0x00;
							NU_SendBuf[3] = 0x00;
							NU_SendBuf[4] = 0x00;

							NU_SendBuf[5] = nfileSize;
							NU_SendBuf[6] = nfileSize>>8;
							NU_SendBuf[7] = nfileSize>>16;
							NU_SendBuf[8] = nfileSize>>24;			

							if (nfileLength == nfileSize)//接收完成，关闭文件
							{
								write(clifd, NU_SendBuf, 9);
								fflush(pFile);
								fclose(pFile);

								printf("\nupgrade exe: File[%s] DownLoad complete!\n", TAR_FILE_NAME);   

								ret = process();//升级包接收完成，执行升级

								while(ret) ;

								pFile = NULL;
								return NULL;
							}
							else if (nfileLength > nfileSize)//没有接收完
							{
								write(clifd, NU_SendBuf, 9);
								nNum = 0;
								continue;
							}
						}
						else
						{
							printf("\nupgrade exe:  recv file error!   symbol is %x\n", *byMsg);
						}
					}
					else
						break;
				}
				else
				{
					break; //interrupt
				}
			}

			printf("upgrade  ser exe: close connect\n");
			close(clifd);
			close(sockfd);
			break;
		}
	}
}

int  PlatformInterface::process()
{
	int iRet = 0;
	FILE *pFile = NULL;
	char Read_Buf[2048];
	char *pPos = NULL;
	iRet = system("tar xvzf "TAR_FILE_NAME" -C /tmp/");
	if (iRet < 0)
	{
		printf("upgrade exe: tar xvzf "TAR_FILE_NAME" -C /tmp/ Faild\n");
		return 0;
	}

	iRet = system("chmod 777 "SH_FILE_NAME" ");
	if (iRet < 0)
	{
		printf("upgrade exe: chmod 777 "SH_FILE_NAME"\n");
		return 0;
	}

	iRet = system("sh "SH_FILE_NAME" ");
	if (iRet < 0)
	{
		printf("upgrade exe: "SH_FILE_NAME" Faild\n");
		return 0;
	}

	printf("#### <%s> iRet=%d\n", __func__, iRet);
	printf("#### <%s> WIFEXITED(status):%d\n", __func__, WIFEXITED(iRet));
	printf("#### <%s> WEXITSTATUS(status):%d\n", __func__, WEXITSTATUS(iRet));

	if ((-1 != iRet) && WIFEXITED(iRet))
	{
		while(0 == WEXITSTATUS(iRet))
		{
			sleep(1);
		}
	}

	//update重启失败，删除update标识文件
    WriteFile(SYS_REBOOT_LOG, "");
	return 1;
}

Tool::TH_RET PlatformInterface::UpdateCmdCreateThread(LPVOID lpContext)
{
	if(lpContext == NULL)
		return NULL;
	PlatformInterface *pPlatformInterface = (PlatformInterface *)lpContext;
	pPlatformInterface->UpdateCommunication();

	return NULL;
}

bool PlatformInterface::CmdUpdate(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 写入升级信息
	char buf[512] = {0};
	snprintf(buf, sizeof(buf), "1 %s %d %s %s", strSendtoIP.c_str(), nSendtoPort, strMsgID.c_str(), "UpdateR");
	WriteFile(SYS_REBOOT_LOG, buf);

	m_hUpdateThread = CZThread::BeginThread(UpdateCmdCreateThread, this);

    return true;
}

bool PlatformInterface::CmdRestart(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;

    // 2.处理命令
    // 重启机器
    if (0 == strncmp(pChannel->value(), "All", strlen("All")))
    {
        // 2.处理命令
        char buf[512] = {0};
        snprintf(buf, sizeof(buf), "1 %s %d %s %s", strSendtoIP.c_str(), nSendtoPort, strMsgID.c_str(), "RestartR");
        WriteFile(SYS_REBOOT_LOG, buf);
        Reboot();

        // 重启之后回复执行结果
        return true;
    }

    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    // 重启通道
    bool bSuccess = false;
    if(nChannelID >= 0 && nChannelID < CHANNEL_MAX_NUM)
    {
        bSuccess = ChannelManager::GetInstance()->ReStartChannel(nChannelID);
    }
    else
    {
        log_error("restart channel %d failed !", nChannelID);
    }

    CBHXml xml;
    if (bSuccess)
    {
        XML_HEAD_SUCCESS("RestartR", strMsgID.c_str());
        XML_END_SUCCESS();
    }
    else
    {
        string strTemp = to_string(PROTOCOL_PARAM_ERROR);
        XML_HEAD_FAILED("RestartR", strMsgID.c_str(), strTemp.c_str(), "Error");
        XML_END_FAILED();
    }

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdFactory(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 2.处理命令 重启之后回复执行结果
    CConfig::GetInstance()->Restore();

    char buf[512] = {0};
    snprintf(buf, sizeof(buf), "1 %s %d %s %s", strSendtoIP.c_str(), nSendtoPort, strMsgID.c_str(), "FactoryR");
    WriteFile(SYS_REBOOT_LOG, buf);
    Reboot();

    return true;
}

bool PlatformInterface::CmdSaveAllCfg(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    CConfig::GetInstance()->SaveConfig();
    CConfig::GetInstance()->SaveChannel();

    CBHXml xml;
    XML_HEAD_SUCCESS("SaveAllCfgR", strMsgID.c_str());
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

int  PlatformInterface::SntpCreatSocket(string strIP)
{
	struct sockaddr_in local;

	m_SocketFd = socket(AF_INET,SOCK_DGRAM,0);

	//绑定本机地址，端口号为123
	local.sin_family = AF_INET;
	local.sin_addr.s_addr = inet_addr(strIP.c_str());
	local.sin_port= htons(m_SNTPPort);

	int iRet = bind (m_SocketFd,(struct sockaddr*)&(local),sizeof(local));
	if ( iRet == -1 ) 
	{
		m_SocketFd = 0;
		printf("SntpProcess bind err\n");
		return -1;
	}
	return 0;
}

int PlatformInterface::SntpRecv(unsigned char *pData,struct SntpNode *pSendNode)
{
	struct timeval timeout = {1, 0};		//1s
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(m_SocketFd, &fds);
	int nRet = -1;

	struct SntpNode RecNode;

	while (1)
	{
		//检查是否有数据到达
		if(-1 == select(m_SocketFd + 1, &fds, NULL, NULL, &timeout))
		{
			printf("RecvSntp select failed !\n");
			return nRet;
		}

		//若没有数据到达，则不再接收，等待下次再发
		if (!FD_ISSET(m_SocketFd, &fds))
		{
			// printf("RecvSntp FD_ISSET failed !\n");
			return nRet;
		}

		//有数据到达
		struct sockaddr_in addrSender;
		int SenderAddrSize = sizeof(addrSender);
		int nCount = recvfrom(m_SocketFd, pData, SOCK_DATA_LEN, 
			0, (struct sockaddr *)&addrSender, (socklen_t *)&SenderAddrSize);


		//校验数据，扔掉底层缓存的校时包
		if (nCount >= sizeof(struct SntpNode))
		{
			memcpy(&RecNode, pData, sizeof(struct SntpNode));
			if ((RecNode.OrgTimestamp.m_unInteger == pSendNode->TransTimestamp.m_unInteger))
			{
				return nCount;
			}
		}	

		nRet = nCount;
	}

	return nRet;
}

Tool::TH_RET PlatformInterface::TH_Sntp(LPVOID lpContext)
{
	if(lpContext == NULL)
		return NULL;
	PlatformInterface *pPlatformInterface = (PlatformInterface *)lpContext;
	pPlatformInterface->SntpProcess();

	return NULL;
}

//添加函数 同步时间
void* PlatformInterface::SntpProcess()
{
	SysConfig &stSysCfg = CConfig::GetInstance()->GetConfig();
	
	unsigned char byData[SOCK_DATA_LEN];
	struct sockaddr_in sendAddr;
	int ret = 0;

//	m_threadId = gettid();
	while (m_bRun)
	{
		if (SNTP_DISABLE == stSysCfg.nEnable || stSysCfg.strTimeServerIP.empty())
		{
			sleep(1);
			continue;
		}

		SntpNode node;
        OS_TimeVal tvCurr;

		if(m_SocketFd <= 0)
		{
			ret = SntpCreatSocket(stSysCfg.strCtrlNetIP);
			if (ret < 0)
			{
				sleep(1);
				continue;
			}
		}

#if 0
		node.LI = 3;
		node.VN = 4;
		node.Mode = 3;		
		node.Stratum = 0;		//故障信息
		node.Poll = 3;			//最大时间间隔为8秒
#else	// support windows xp sntp
		node.LI = 3;
		node.VN = 3;
		node.Mode = 3;
		node.Stratum = 0;
		node.Poll = 0x11;			//最大时间间隔为17秒
#endif		
		node.Precision = 0xFA;	//系统时钟精度为2的-16次方
		node.RootDelay = 1.0;		//与主时钟源的延迟
		node.RootDispersion	= 1.0;	
		node.RefIndentifier[0] = 'G';
		node.RefIndentifier[1] = 'P';
		node.RefIndentifier[2] = 'S';
		node.RefIndentifier[3] = '\0';
		node.RefTimestamp.m_unInteger = 0x00;
		node.RefTimestamp.m_unFractional = 0x00;
		node.OrgTimestamp.m_unInteger = 0x00;
		node.OrgTimestamp.m_unFractional = 0x00;
		node.ReceiveTimestamp.m_unInteger = 0x00;
		node.ReceiveTimestamp.m_unFractional = 0x00;

		//获取当前时间
		//can not use gettimeofday(&tvCurr, NULL);
        OS_Time::GetWallTime(&tvCurr);  //modify lh

		//将当前时间转换为相对于1900年1月1日之后的秒数，并赋给发送时间
		node.TransTimestamp.m_unInteger = htonl(tvCurr.tv_sec + INIT_TIME - CHINA_TIME_FIELD);
		node.TransTimestamp.m_unFractional = htonl(tvCurr.tv_usec);//0x00;

		memcpy(byData, &node, sizeof(node));
		//设置sntp服务器的地址

		//设置发送地址
		memset(&sendAddr,0,sizeof(sendAddr));
		sendAddr.sin_family = AF_INET; 
		sendAddr.sin_addr.s_addr = inet_addr(stSysCfg.strTimeServerIP.c_str()); 
		sendAddr.sin_port = htons(m_SNTPPort);

		//发送时钟请求给sntp服务器
		int iRet = sendto(m_SocketFd, byData, sizeof(node),0,
			(struct sockaddr*)&sendAddr,sizeof(sendAddr));
		if (iRet < 0)
		{
			close(m_SocketFd);
			m_SocketFd = -1;
			perror("Send RTC Data");
			printf("[%d]Send RTC Data to [%s:%d] Error <%d>!\n",m_SocketFd, m_byaServerIp,m_SNTPPort, iRet);
			sleep(3);
			continue;
		}

		//等待sntp服务器的返回
		iRet = SntpRecv(byData, &node);
		if (iRet <= 0)
		{
			close(m_SocketFd);
			m_SocketFd = -1;
			sleep(3);
			continue;
		}

		//转换为node
		memcpy(&node, byData, sizeof(node));

		//计算时间差
		//can not use gettimeofday(&tvCurr, NULL);
        OS_Time::GetWallTime(&tvCurr);  //modify liuhao
		int nDiffTime = ntohl(node.TransTimestamp.m_unInteger) - INIT_TIME - tvCurr.tv_sec /*+ CHINA_TIME_FIELD*/;

		tvCurr.tv_sec =tvCurr.tv_sec + nDiffTime ;
		tvCurr.tv_usec =  0;

		//当时间差大于1秒或者间隔10min或者同步时间不成功，则进行同步
		if ((abs(nDiffTime) > 1) || (m_uTimeout10S_Count <= 0))
		{
			m_uUpdateTime = tvCurr.tv_sec;
			m_uTimeout10S_Count = m_uGap/10;// 10 seconds
			// can not use settimeofday , because it will chang sys time, so gettimeofday time may be back
            OS_Time::SetWallTimeDiff(nDiffTime);
		}

		if(m_uTimeout10S_Count > 0) 
			m_uTimeout10S_Count--;

        OS_Thread::Sleep(10);
	}

	return NULL;
}

bool PlatformInterface::CmdSetTimeServerAddr(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML

	/*
	<Data>
		<Addr>172.16.23.12</Addr>
	</Data>
	*/

    rapidxml::xml_node<> * pAddr = pData->first_node("Addr");

    SysConfig &stSysCfg = CConfig::GetInstance()->GetConfig();
    stSysCfg.strTimeServerIP = pAddr->value();

    // 2.处理命令
    CBHXml xml;
    XML_HEAD_SUCCESS("SetTimeServerAddrR", strMsgID.c_str());
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdSetTcorderInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    bool inFormatCorrect = true;
    // 这里channel可能是多个 共同完成一个任务

    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;

    string strChannels = pChannel->value();

    vector<int> vecChannel;
    int nFindStart = 0;
    while (1)
    {
        size_t nSeparatorStart = strChannels.find(",", nFindStart);
        if (string::npos == nSeparatorStart)
        {
            vecChannel.push_back(atoi(strChannels.substr(nFindStart).c_str()) - 1);
            break;
        }

        vecChannel.push_back(atoi(strChannels.substr(nFindStart, nSeparatorStart - nFindStart).c_str()) - 1);
        nFindStart = nSeparatorStart + 1;
    }

    // In
    TSInAddr stTSInAddr;
    TSInParam stTSInParam;

    rapidxml::xml_node<char> * pIn = pData->first_node("Channel");
    if (pIn)
    {
        rapidxml::xml_node<char> * pSrcIP = pIn->first_node("SrcIP");
        ParseAddrToIPPort(pSrcIP->value(), stTSInAddr.strIP, stTSInAddr.nPort);

        rapidxml::xml_node<char> * pVCode = pIn->first_node("VCode");
        stTSInParam.nVCode = atoi(pVCode->value());
        if (stTSInParam.nVCode < 1 || stTSInParam.nVCode > 5)
        {
            inFormatCorrect = false;
        }

        rapidxml::xml_node<char> * pACode = pIn->first_node("ACode");
        stTSInParam.nACode = atoi(pACode->value());
        if (stTSInParam.nACode < 1 || stTSInParam.nACode > 4)
        {
            inFormatCorrect = false;
        }

        rapidxml::xml_node<char> * pServiceID = pIn->first_node("ServiceID");
        stTSInParam.nServiceID = atoi(pServiceID->value());

        rapidxml::xml_node<char> * pVPID = pIn->first_node("VPID");
        stTSInParam.nVPID = atoi(pVPID->value());

        rapidxml::xml_node<char> * pAPIDList = pIn->first_node("APIDList");
        for (rapidxml::xml_node<char> * pAPID= pAPIDList->first_node("APID");
             pAPID != NULL;
             pAPID = pAPID->next_sibling())
        {
            stTSInParam.vecAPID.push_back(atoi(pAPID->value()));
        }

        rapidxml::xml_node<char> * pServiceType = pIn->first_node("ServiceType");
        stTSInParam.nServiceType = atoi(pServiceType->value());
    }

    TSOutParam stTSOutParam;
    rapidxml::xml_node<char> * pOut = pData->first_node("Out");
    if (pOut)
    {
        rapidxml::xml_node<char> * pVCode = pOut->first_node("VCode");
        stTSOutParam.nVCode = atoi(pVCode->value()); //视频输出  支持 H264 H265
        if ((stTSOutParam.nVCode != 2) && (stTSOutParam.nVCode != 5))
        {
            stTSOutParam.nVCode = 2;
        }

        rapidxml::xml_node<char> * pACode = pOut->first_node("ACode");
        stTSOutParam.nACode = atoi(pACode->value());	//音频暂时只支持AAC格式
        stTSOutParam.nACode = 1;

        rapidxml::xml_node<char> * pWidth = pOut->first_node("Width");
        stTSOutParam.nWidth = atoi(pWidth->value());
        if (stTSOutParam.nWidth < 0)
        {
            stTSOutParam.nWidth = MIN_DISPLAY_WIDTH_HD;
        }
        if (stTSOutParam.nWidth > MAX_DISPLAY_WIDTH_HD)
        {
            stTSOutParam.nWidth = MAX_DISPLAY_WIDTH_HD;
        }

        rapidxml::xml_node<char> * pHeight = pOut->first_node("Height");
        stTSOutParam.nHeight = atoi(pHeight->value());
        if (stTSOutParam.nHeight < 0)
        {
            stTSOutParam.nHeight = MIN_DISPLAY_HEIGHT_HD;
        }
        if (stTSOutParam.nHeight > MAX_DISPLAY_HEIGHT_HD)
        {
            stTSOutParam.nWidth = MAX_DISPLAY_HEIGHT_HD;
        }

        rapidxml::xml_node<char> * pFrameRate = pOut->first_node("FrameRate");
        stTSOutParam.nFrameRate = atoi(pFrameRate->value());

        rapidxml::xml_node<char> * pVRate = pOut->first_node("VRate");
        stTSOutParam.nVRate = atoi(pVRate->value());

        rapidxml::xml_node<char> * pARate = pOut->first_node("ARate");
        stTSOutParam.nARate = atoi(pARate->value());

    }

    // 保存到配置文件模块
    for(auto it = vecChannel.begin(); it != vecChannel.end(); ++it)
    {
        int nChannelID = *it;
        if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
            continue;

        SysChannel &stSysChannel = CConfig::GetInstance()->GetChannel(nChannelID);
        stSysChannel.stTSInAddr = stTSInAddr;
        stSysChannel.stTSInParam = stTSInParam;
        stSysChannel.stTSOutParam = stTSOutParam;
    }


    // 2.处理命令
    bool bSuccess = false;
    if (inFormatCorrect)
    {
        bSuccess = ChannelManager::GetInstance()->SetChannelTransParam(vecChannel, stTSInAddr, stTSInParam, stTSOutParam);
    }

    CBHXml xml;
    if(bSuccess)
    {
        XML_HEAD_SUCCESS("SetTcorderInfoR", strMsgID.c_str());
        XML_END_SUCCESS();
    }
    else
    {
        string strTemp = to_string(1);
        XML_HEAD_FAILED("SetTcorderInfoR", strMsgID.c_str(), strTemp.c_str(), "Error");
        XML_END_FAILED();
    }

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdGetTcorderInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;
    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
        return false;

    // 2.处理命令
    TSInAddr stTSInAddr;
    TSInParam stTSInParam;
    TSOutParam stTSOutParam;
    ChannelManager::GetInstance()->GetChannelTransParam(nChannelID, stTSInAddr, stTSInParam, stTSOutParam);

    CBHXml xml;
    XML_HEAD_SUCCESS("GetTcorderInfoR", strMsgID.c_str());
    xml.AddElem("Data");
    xml.IntoElem();

    // In
    xml.AddElem("In");
    xml.IntoElem();
    string strTemp =stTSInAddr.strIP + ":" + to_string(stTSInAddr.nPort); 
    xml.AddElem("SrcIP", strTemp.c_str());
    strTemp = to_string(stTSInParam.nVCode);
    xml.AddElem("VCode", strTemp.c_str());
    strTemp = to_string(stTSInParam.nACode);
    xml.AddElem("ACode", strTemp.c_str());
    strTemp = to_string(stTSInParam.nServiceID);
    xml.AddElem("ServiceID", strTemp.c_str());
    strTemp = to_string(stTSInParam.nVPID);
    xml.AddElem("VPID", strTemp.c_str());

    xml.AddElem("APIDList");
    xml.IntoElem();
    for(auto it=stTSInParam.vecAPID.begin(); it!=stTSInParam.vecAPID.end(); ++it)
    {
        strTemp = to_string(*it);
        xml.AddElem("APID", strTemp.c_str());
    }
    xml.OutOfElem();

    strTemp = to_string(stTSInParam.nServiceType);
    xml.AddElem("ServiceType", strTemp.c_str());
    xml.OutOfElem();

    // Out
    xml.AddElem("Out");
    xml.IntoElem();
    strTemp = to_string(stTSOutParam.nVCode);
    xml.AddElem("VCode", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nACode);
    xml.AddElem("ACode", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nWidth);
    xml.AddElem("Width", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nHeight);
    xml.AddElem("Height", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nFrameRate);
    xml.AddElem("FrameRate", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nVRate);
    xml.AddElem("VRate", strTemp.c_str());
    strTemp = to_string(stTSOutParam.nARate);
    xml.AddElem("ARate", strTemp.c_str());
    xml.OutOfElem();

    xml.OutOfElem();    // out of data
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdSetOSDInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;
    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
        return false;

    SysChannel &stSysChannel = CConfig::GetInstance()->GetChannel(nChannelID);

    rapidxml::xml_node<char> * pText = pChannel->first_node("Text");
    char * strText = pText->value();
    strText = helper_iconv("gb2312", "utf-8", strText);

    stSysChannel.stOSD.strText = string(strText, strlen(strText));

    rapidxml::xml_node<char> * pFontSize = pChannel->first_node("FontSize");
    stSysChannel.stOSD.nFontSize = atoi(pFontSize->value());

    rapidxml::xml_node<char> * pPosition = pChannel->first_node("Position");
    stSysChannel.stOSD.nPosition = atoi(pPosition->value());

    rapidxml::xml_node<char> * pAntiColor = pChannel->first_node("AntiColor");
    stSysChannel.stOSD.nAntiColor = atoi(pAntiColor->value());

    rapidxml::xml_node<char> * pAlign = pChannel->first_node("Align");
    stSysChannel.stOSD.nAlign = atoi(pAlign->value());

    rapidxml::xml_node<char> * pType = pChannel->first_node("Type");
    stSysChannel.stOSD.nType = atoi(pType->value());

    // 2.处理命令
    ChannelManager::GetInstance()->SetChannelOSDParam(nChannelID, stSysChannel.stOSD);
    CBHXml xml;
    XML_HEAD_SUCCESS("SetTcorderInfoR", strMsgID.c_str());
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

bool PlatformInterface::CmdGetOSDInfo(rapidxml::xml_node<char> * pData, string &strMsgID, string& strSendtoIP, int nSendtoPort)
{
    // 1.解析XML
    rapidxml::xml_node<char> * pChannel = pData->first_node("Channel");
    if (NULL == pChannel) return false;
    int nChannelID = atoi(pChannel->value());
    nChannelID--;

    if (nChannelID < 0 || nChannelID >= CHANNEL_MAX_NUM)
        return false;

    // 2.处理命令
    OsdInfo stOSD;
    ChannelManager::GetInstance()->GetChannelOSDParam(nChannelID, stOSD);

    CBHXml xml;
    XML_HEAD_SUCCESS("GetOSDInfoR", strMsgID.c_str());
    xml.AddElem("Data");
    xml.IntoElem();

    char * strText = (char *)stOSD.strText.c_str();
    strText = helper_iconv("utf-8", "gb2312", strText);
    xml.AddElem("Text", (const char *)strText);

    string strTemp = to_string(stOSD.nFontSize);
    xml.AddElem("FontSize", strTemp.c_str());
    strTemp = to_string(stOSD.nPosition);
    xml.AddElem("Position", strTemp.c_str());
    strTemp = to_string(stOSD.nAntiColor);
    xml.AddElem("AntiColor", strTemp.c_str());
    strTemp = to_string(stOSD.nAlign);
    xml.AddElem("Align", strTemp.c_str());
    strTemp = to_string(stOSD.nType);
    xml.AddElem("Type", strTemp.c_str());

    xml.OutOfElem();    // out of data
    XML_END_SUCCESS();

    const char * strXml = xml.GetDoc();

    // 3.发送HTTP响应
    AddMsgToPlatform(strXml, strSendtoIP, nSendtoPort);

    return true;
}

void PlatformInterface::RestartReport()
{
    // 检查SYS_REBOOT_LOG文件 如果内容为1 说明刚才是命令重启 需要回复消息
    char *pchContext = ReadFile(SYS_REBOOT_LOG);
    if (!pchContext)
    {
        printf("Open SYS_REBOOT_LOG File Failed.\n");
        return ;
    }

    // 1 172.17.9.150(IP) 1000(port) 10000(msgid) RestartR/FactoryR
    string strRebootLog(pchContext);
    SAFE_FREE(pchContext);

    int nStartPos = 0;
    int nEndPos = strRebootLog.find(" ");
    if(string::npos == nEndPos)
        return ;
    int nReport = atoi(strRebootLog.substr(nStartPos, nEndPos - nStartPos).c_str());
    if(1 != nReport)
        return ;

    nStartPos = nEndPos + 1;
    nEndPos = strRebootLog.find(" ", nStartPos);
    if(string::npos == nEndPos)
        return ;
    string strSendToIP = strRebootLog.substr(nStartPos, nEndPos - nStartPos);

    nStartPos = nEndPos + 1;
    nEndPos = strRebootLog.find(" ", nStartPos);
    if(string::npos == nEndPos)
        return ;
    int nSendToPort = atoi(strRebootLog.substr(nStartPos, nEndPos - nStartPos).c_str());

    nStartPos = nEndPos + 1;
    nEndPos = strRebootLog.find(" ", nStartPos);
    if(string::npos == nEndPos)
        return ;
    string strMsgID = strRebootLog.substr(nStartPos, nEndPos - nStartPos);

    nStartPos = nEndPos + 1;
    string strType = strRebootLog.substr(nStartPos);

    // 构造字符串
    CBHXml xml;
    XML_HEAD_SUCCESS(strType.c_str(), strMsgID.c_str());
    XML_END_SUCCESS();
    const char * strXml = xml.GetDoc();

    // 发送HTTP响应
    AddMsgToPlatform(strXml, strSendToIP, nSendToPort);

    // 将文件内容删除
    WriteFile(SYS_REBOOT_LOG, "");
}

void PlatformInterface::MultiBoardCheck(const SysConfig &stSysConfig)
{
    // 组装字符串
    unsigned char buffer[MAX_BUFFER_LEN] = {0};
    timeval curTv;
    gettimeofday(&curTv, NULL);

    buffer[0] = HEAD_SYNC;		                        // sync
    buffer[LOAD_TYPE_OFFSET] = 0x01;	                // data type
    buffer[LOAD_LEN_OFFSET] = BOARD_CHECK_DATA_LEN;	    // data length

    unsigned char *pos = &buffer[LOAD_DATA_OFFSET];
    *pos = BOARD_CHASSIS;           // 机框号
    pos++;
    *pos = BOARD_SLOT;              // 卡槽号
    pos += 2;
    *pos = BOARD_TYPE;
    pos++;
    *pos = 45;          // 温度 暂时无法获取
    pos++;
    *pos = 0;	        // 板卡状态 0正常 1异常
    pos++;
    *pos = 0;	        // 异常代号
    pos += 2;
    *pos = curTv.tv_sec & 0xff;
    pos++;
    *pos = (curTv.tv_sec >> 8) & 0xff;
    pos++;
    *pos = (curTv.tv_sec >> 16) & 0xff;
    pos++;
    *pos = (curTv.tv_sec >> 24) & 0xff;
    pos++;

    CalcCRC(buffer, BOARD_CHECK_MSG_LEN);

    //printf("MultiBoardCheck:\n");
    //for(int i=0; i<BOARD_CHECK_MSG_LEN; ++i)
    //{
    //    printf("%02x ", buffer[i]);
    //}
    //printf("\n");
    
    // 发送消息
    OS_SockAddr addrTo(stSysConfig.strOutBoardCheckIP.c_str(), stSysConfig.nOutBoardCheckPort);
    int nRet = m_sockMulitPost.SendTo(stSysConfig.strCtrlNetIP.c_str(), buffer, BOARD_CHECK_MSG_LEN, addrTo);

    if (nRet != BOARD_CHECK_MSG_LEN)
    {
        log_error("Send(%s:%d %s) Failed\n", stSysConfig.strOutBoardCheckIP.c_str(), stSysConfig.nOutBoardCheckPort, 
                    stSysConfig.strCtrlNetIP.c_str());
    }
}

void PlatformInterface::MultiChannelResource(const SysConfig &stSysConfig)
{
    // 组装字符串
    unsigned char buffer[MAX_BUFFER_LEN] = {0};

    buffer[0] = HEAD_SYNC;		                        // sync
    buffer[LOAD_TYPE_OFFSET] = 0x02;	                // data type
    buffer[LOAD_LEN_OFFSET] = CH_RESOURCE_DATA_LEN;	    // data length

    unsigned char *pos = &buffer[LOAD_DATA_OFFSET];
    *pos = BOARD_CHASSIS;	            // 机框号
    pos++;
    *pos = BOARD_SLOT;		            // 卡槽号
    pos++;
    *pos = stSysConfig.nResourceSum;	//通道数量
    pos++;
    *pos = BOARD_TYPE;
    pos++;

    //交互指令监听IP
    unsigned char ip[4] = {0};
    sscanf(stSysConfig.strCtrlNetIP.c_str(), "%d.%d.%d.%d", (unsigned int *)&ip[0], (unsigned int *)&ip[1], (unsigned int *)&ip[2], (unsigned int *)&ip[3]);
    for(int i = 0; i < 4; i++)
    {
        *pos = ip[i];
        pos++;
    }

    //交互指令监听端口
    *pos = stSysConfig.nCtrlPort & 0xff;
    pos++;
    *pos = (stSysConfig.nCtrlPort >> 8) & 0xff;
    pos += 3;

    // 转码各格式所需通道数确定 bit0-bit3标清  bit4-bit7高清
    *pos = (stSysConfig.nResourceMpeg2SDNeeds & 0x0f) | ((stSysConfig.nResourceMpeg2HDNeeds << 4) & 0xf0);
    pos++;
    *pos = (stSysConfig.nResourceH264SDNeeds & 0x0f) | ((stSysConfig.nResourceH264HDNeeds << 4) & 0xf0);
    pos++;
    *pos = (stSysConfig.nResourceAVSSDNeeds & 0x0f) | ((stSysConfig.nResourceAVSHDNeeds << 4) & 0xf0);
    pos++;
    *pos = (stSysConfig.nResourceH265SDNeeds & 0x0f) | ((stSysConfig.nResourceH265HDNeeds << 4) & 0xf0);
    pos++;

    CalcCRC(buffer, CH_RESOURCE_MSG_LEN);

    //printf("MultiChannelResource:\n");
    //for(int i=0; i<CH_RESOURCE_MSG_LEN; ++i)
    //{
    //    printf("%02x ", buffer[i]);
    //}
    //printf("\n");

    // 发送消息
    OS_SockAddr addrTo(stSysConfig.strOutResourceIP.c_str(), stSysConfig.nOutResourcePort);
    int nRet = m_sockMulitPost.SendTo(stSysConfig.strCtrlNetIP.c_str(), buffer, CH_RESOURCE_MSG_LEN, addrTo);

    if(nRet != CH_RESOURCE_MSG_LEN)
    {
        log_error("Send(%s:%d %s) Failed\n", stSysConfig.strOutResourceIP.c_str(), stSysConfig.nOutResourcePort, 
            stSysConfig.strCtrlNetIP.c_str());
    }
}

void PlatformInterface::MultiAVErrorStatus(const SysConfig &stSysConfig)
{
    // 组装字符串
    unsigned char buffer[MAX_BUFFER_LEN] = {0};

    buffer[0] = HEAD_SYNC;		                        // sync
    buffer[LOAD_TYPE_OFFSET] = 0x05;	                // data type
    buffer[LOAD_LEN_OFFSET] = AV_ERR_STATUS_DATA_LEN;	// data length

    for(int nIndex=0; nIndex<CHANNEL_MAX_NUM; ++nIndex)
    {
        // 需要获取每一个通道的组播地址
        unsigned short sWarnVedioStatus = 0;
        unsigned short sWarnAudioStatus = 0;
        string strOutIP("");
        int nOutPort = 0;

        // 获取告警状态
        if(!ChannelManager::GetInstance()->GetMonitorStatus(nIndex, sWarnVedioStatus, sWarnAudioStatus, strOutIP, nOutPort))
            continue;

        // 组装字符串
        unsigned char *pos = &buffer[LOAD_DATA_OFFSET];
        *pos = sWarnVedioStatus & 0xff;
        pos++;
        *pos = (sWarnVedioStatus >> 8) & 0xff;
        pos++;

        *pos = sWarnAudioStatus & 0xff;
        pos++;
        *pos = (sWarnAudioStatus >> 8) & 0xff;
        pos++;

        timeval curTv;
        gettimeofday(&curTv, NULL);
        *pos = curTv.tv_sec & 0xff;
        pos++;
        *pos = (curTv.tv_sec >> 8) & 0xff;
        pos++;
        *pos = (curTv.tv_sec >> 16) & 0xff;
        pos++;
        *pos = (curTv.tv_sec >> 24) & 0xff;
        pos++;

        CalcCRC(buffer, AV_ERR_STATUS_MSG_LEN);

        //printf("MultiAVErrorStatus(Index:%d):\n", nIndex);
        //for(int i=0; i<AV_ERR_STATUS_MSG_LEN; ++i)
        //{
        //    printf("%02x ", buffer[i]);
        //}
        //printf("\n");

        // 发送消息
        OS_SockAddr addrTo(strOutIP.c_str(), nOutPort);
        int nRet = m_sockMulitPost.SendTo(stSysConfig.strCtrlNetIP.c_str(), buffer, AV_ERR_STATUS_MSG_LEN, addrTo);

        if(nRet != AV_ERR_STATUS_MSG_LEN)
        {
            log_error("Index:%d Send(%s:%d %s) Failed\n", nIndex, strOutIP.c_str(), nOutPort,
                stSysConfig.strCtrlNetIP.c_str());
        }
    }
}

void PlatformInterface::InitCRC()
{
    unsigned int temp;
    for(int i = 0; i < 256; i++)
    {
        temp = i << 24;
        for(int j = 0; j < 8; j++)
        {
            if(temp & 0x80000000)
            {
                temp = (temp << 1)^0x04c11db7;
            }
            else
            {
                temp = temp << 1;
            }
        }
        m_arrCrc32Table[i] = temp;
    }
}

void PlatformInterface::CalcCRC(unsigned char *pData, unsigned int uDataLen)
{
    unsigned int uCRC = 0xffffffff;

    for (unsigned int i = 0; i < uDataLen - MSG_CRC_LEN; i++)
        uCRC = (uCRC << 8) ^ m_arrCrc32Table[(uCRC >> 24) ^ (pData[i])];

    pData[uDataLen - 4] = (uCRC >> 24) & 0xff;
    pData[uDataLen - 3] = (uCRC >> 16) & 0xff;
    pData[uDataLen - 2] = (uCRC >> 8) & 0xff;
    pData[uDataLen -1] = uCRC & 0xff;
}

bool ParseAddrToIPPort(string strAddr, string &strIP, int &nPort)
{
    size_t nSeparatorPos = strAddr.find(":");
    if(string::npos == nSeparatorPos)
    {
        return false;
    }

    strIP = strAddr.substr(0, nSeparatorPos);
    nPort = atoi(strAddr.substr(nSeparatorPos + 1).c_str());
    return true;
}

void Reboot()
{
    sync();
    sleep(3);
    reboot(RB_AUTOBOOT);
}

//读写文件
char *ReadFile(const char *cstrPath)
{
    // 1. 从本地读取json配置文件到字符串
    char *pchContext = NULL;
    CFileEx::SetCurDirectory(CFileEx::GetExeDirectory().c_str());

    int nFd = open(cstrPath, O_RDONLY);
    if (-1 != nFd)
    {
        size_t nLen = lseek(nFd, 0L, SEEK_END);
        lseek(nFd, 0L, SEEK_SET);

        pchContext = (char *)malloc(nLen + 1);
        memset(pchContext, 0x00, nLen + 1);
        read(nFd, pchContext, nLen);
        pchContext[nLen] = '\0';

        close(nFd);
        nFd = -1;
    }

    return pchContext;
}

bool WriteFile(const char *cstrPath, const char *cstrContext)
{
    CFileEx::SetCurDirectory(CFileEx::GetExeDirectory().c_str());

    // 使用不带缓存的IO
    int nFd = open(cstrPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (-1 != nFd)
    {
        write(nFd, cstrContext, strlen(cstrContext));
        close(nFd);
        nFd = -1;
    }
    else
        return false;

    return true;
}

char *helper_iconv(const char *encFrom, const char *encTo, char *in)
{
    static char buf[2048], *sin, *sout;
    int lenin, lenout, ret;
    iconv_t c_pt;

    memset(buf, 0, 2048);

    if ((c_pt = iconv_open(encTo, encFrom)) == (iconv_t)-1)
    {
        return NULL;
    }
    iconv(c_pt, NULL, NULL, NULL, NULL);
    lenin  = strlen(in) + 1;
    lenout = 2048;
    sin    = (char *)in;
    sout   = buf;
    ret = iconv(c_pt, &sin, (size_t *)&lenin, &sout, (size_t *)&lenout);
    if (ret == -1)
    {
        iconv_close(c_pt);
        return NULL;
    }
    iconv_close(c_pt);
    return buf;
}
