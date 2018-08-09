#include "SysConfig.h"
#include "FileEx.h"
#include "BHXml.h"

static CConfig *g_pConfig = NULL;

static const char * SYS_CONFIG_FILE = "Config/Config.xml";
static const char * SYS_CHANNEL_FILE = "Config/Channel.xml";
static const char * SYS_MONITOR_FILE = "Config/Monitor.xml";

/*=SYSTEM STATE INFO====================================================*/
/* these are for calculating cpu state percentages */

static long long cp_time[NCPUSTATES];
static long long cp_old[MAXCORES+1][NCPUSTATES];
static long long cp_diff[NCPUSTATES];


/* these are for passing data back to the machine independant portion */

static int cpu_states[MAXCORES+1][NCPUSTATES];
static int memory_stats[NMEMSTATS];

/*======================================================================*/

static inline char *
skip_ws(const char *p)
{
	while (isspace(*p)) p++;
	return (char *)p;
}

static inline char *
skip_token(const char *p)
{
	while (isspace(*p)) p++;
	while (*p && !isspace(*p)) p++;
	return (char *)p;
}

static inline char *
skip_line(const char *p)
{
	while (*p && (*p != '\r' && *p != '\n')) p++;
	while (*p && (*p == '\r' || *p == '\n')) p++;
	return (char *)p;
}

static int get_link_status(char *eth)
{
	int fd, len;
	char path[256];
	char c;

	snprintf(path, 256, "/sys/class/net/%s/carrier", eth);
	fd = open(path, O_RDONLY);
	if (fd < 0) return 0;
	len = read(fd, &c, 1);
	close(fd);

	return (c == '1' ? 1 : 0);
}

/*
 *  percentages(cnt, out, new, old, diffs) - calculate percentage change
 *  between array "prv" and "cur", putting the percentages i "out".
 *  "cnt" is size of each array and "diffs" is used for scratch space.
 *  The array "prv" is updated on each call.
 *  The routine assumes modulo arithmetic.  This function is especially
 *  useful on BSD mchines for calculating cpu state percentages.
 */

static long long percentages(int cnt, int *out, long long *cur, long long *prv, long long *diffs)
{
    int i;
    long long change;
    long long total_change;
    long long *dp;
    long long half_total;

    /* initialization */
    total_change = 0;
    dp = diffs;

    /* calculate changes for each state and the overall change */
    for (i = 0; i < cnt; i++)
    {
		if ((change = *cur - *prv) < 0)
		{
			/* this only happens when the counter wraps */
			change = (int)
			((unsigned long long)*cur-(unsigned long long)*cur);
		}
		total_change += (*dp++ = change);
		*prv++ = *cur++;
    }

    /* avoid divide by zero potential */
    if (total_change == 0)
    {
		total_change = 1;
    }

    /* calculate percentages based on overall change, rounding up */
    half_total = total_change / 2L;
    for (i = 0; i < cnt; i++)
    {
		*out++ = (int)((*diffs++ * 10000 + half_total) / total_change);
    }

    /* return the total in case the caller wants to use it */
    return(total_change);
}

static bool ParseAddrToIPPort(string strAddr, string &strIP, int &nPort);   // ToDo

SysConfig::SysConfig(): strCtrlNet("eth0"), nCtrlPort(0), strDataNet("eth0"), strCtrlNetIP(""), strDataNetIP(""), strOutBoardCheckIP("224.1.1.2"), nOutBoardCheckPort(5001), strOutResourceIP("224.1.1.5"), nOutResourcePort(5001),
    nResourceSum(0), nResourceMpeg2SDNeeds(0), nResourceMpeg2HDNeeds(0), nResourceH264SDNeeds(0), nResourceH264HDNeeds(0), nResourceH265SDNeeds(0), nResourceH265HDNeeds(0), 
    nResourceAVSSDNeeds(0), nResourceAVSHDNeeds(0), strOEM("bohui"), strCardID(""), strTimeServerIP(""), nTimeServerPort(0), nEnable(1)
{
    mapNetInfo.clear();
}

SysConfig::SysConfig(const SysConfig &node)
{
    strCtrlNet = node.strCtrlNet;
    nCtrlPort = node.nCtrlPort;
    strDataNet = node.strDataNet;
    strCtrlNetIP = node.strCtrlNetIP;
    strDataNetIP = node.strDataNetIP;

    strOutBoardCheckIP = node.strOutBoardCheckIP;
    nOutBoardCheckPort = node.nOutBoardCheckPort;
    strOutResourceIP = node.strOutResourceIP;
    nOutResourcePort = node.nOutResourcePort;

    nResourceSum = node.nResourceSum;
    nResourceMpeg2SDNeeds = node.nResourceMpeg2SDNeeds;
    nResourceMpeg2HDNeeds = node.nResourceMpeg2HDNeeds;
    nResourceH264SDNeeds = node.nResourceH264SDNeeds;
    nResourceH264HDNeeds = node.nResourceH264HDNeeds;
    nResourceH265SDNeeds = node.nResourceH265SDNeeds;
    nResourceH265HDNeeds = node.nResourceH265HDNeeds;
    nResourceAVSSDNeeds = node.nResourceAVSSDNeeds;
    nResourceAVSHDNeeds = node.nResourceAVSHDNeeds;

    strOEM = node.strOEM;
    strCardID = node.strCardID;

    strTimeServerIP = node.strTimeServerIP;
    nTimeServerPort = node.nTimeServerPort;
	nEnable = node.nEnable;

    mapNetInfo.clear();
    for(auto it = node.mapNetInfo.begin(); it != node.mapNetInfo.end(); ++it)
    {
        const NetInfo &stNetInfo = it->second;
        mapNetInfo.insert(make_pair(stNetInfo.strName, stNetInfo));
    }
}

SysConfig & SysConfig::operator = (const SysConfig & node)
{
    strCtrlNet = node.strCtrlNet;
    nCtrlPort = node.nCtrlPort;
    strDataNet = node.strDataNet;
    strCtrlNetIP = node.strCtrlNetIP;
    strDataNetIP = node.strDataNetIP;

    strOutBoardCheckIP = node.strOutBoardCheckIP;
    nOutBoardCheckPort = node.nOutBoardCheckPort;
    strOutResourceIP = node.strOutResourceIP;
    nOutResourcePort = node.nOutResourcePort;

    nResourceSum = node.nResourceSum;
    nResourceMpeg2SDNeeds = node.nResourceMpeg2SDNeeds;
    nResourceMpeg2HDNeeds = node.nResourceMpeg2HDNeeds;
    nResourceH264SDNeeds = node.nResourceH264SDNeeds;
    nResourceH264HDNeeds = node.nResourceH264HDNeeds;
    nResourceH265SDNeeds = node.nResourceH265SDNeeds;
    nResourceH265HDNeeds = node.nResourceH265HDNeeds;
    nResourceAVSSDNeeds = node.nResourceAVSSDNeeds;
    nResourceAVSHDNeeds = node.nResourceAVSHDNeeds;

    strOEM = node.strOEM;
    strCardID = node.strCardID;

    strTimeServerIP = node.strTimeServerIP;
    nTimeServerPort = node.nTimeServerPort;
	nEnable = node.nEnable;

    mapNetInfo.clear();
    for(auto it = node.mapNetInfo.begin(); it != node.mapNetInfo.end(); ++it)
    {
        const NetInfo &stNetInfo = it->second;
        mapNetInfo.insert(make_pair(stNetInfo.strName, stNetInfo));
    }

    return *this;
}


CConfig::CConfig(void)
{
	m_mtxLock.Init();
}

CConfig::~CConfig(void)
{
}

CConfig* CConfig::GetInstance() 
{
    if (g_pConfig == NULL)
    {
        g_pConfig = new CConfig();
    }

    return g_pConfig;
}

void CConfig::DelInstance()
{
    if (g_pConfig != NULL)
    {
        SAFE_DELETE(g_pConfig);
    }
}

bool CConfig::LoadConfig()
{
    //打开可执行文件目录下的Config.xml
    std::string strPath = CFileEx::GetExeDirectory() + CFileEx::Separator() + SYS_CONFIG_FILE;

    char * pData = NULL;

    if (!ReadFile(strPath.c_str(), pData)) return false;

    rapidxml::xml_document<> doc;
    try {
        // parse会改变 szXml 中的内容
        doc.parse<0>(pData);
    }
    catch (...)
    {
        printf("----err xml----\n%s\n----------\n", pData);
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_node<> * pSysConfig = doc.first_node("SysConfig");
    if (pSysConfig == NULL) {
        log_error("cannot find node SysConfig");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_node<char> * pIPInfo = pSysConfig->first_node("IPInfo");
    if (NULL == pIPInfo) {
        log_error("cannot find node IPInfo");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_attribute<char> * pCtrlNet = pIPInfo->first_attribute("CtrlNet");
    if (pCtrlNet) m_stSysCfg.strCtrlNet = pCtrlNet->value();

    rapidxml::xml_attribute<char> * pCtrlPort = pIPInfo->first_attribute("CtrlPort");
    if (pCtrlPort) m_stSysCfg.nCtrlPort = atoi(pCtrlPort->value());

    rapidxml::xml_attribute<char> * pDataNet = pIPInfo->first_attribute("DataNet");
    if (pDataNet) m_stSysCfg.strDataNet = pDataNet->value();

    for (rapidxml::xml_node<char> * pEth = pIPInfo->first_node("eth");
        pEth != NULL;
        pEth = pEth->next_sibling())
    {
        NetInfo stNetInfo;

        rapidxml::xml_attribute<char> * pName = pEth->first_attribute("Name");
        if (pName) stNetInfo.strName = pName->value();

        rapidxml::xml_attribute<char> * pIP = pEth->first_attribute("IP");
        if (pIP) stNetInfo.strIP = pIP->value();

        rapidxml::xml_attribute<char> * pNetmask = pEth->first_attribute("Netmask");
        if (pNetmask) stNetInfo.strNetmask = pNetmask->value();

        rapidxml::xml_attribute<char> * pGateway = pEth->first_attribute("Gateway");
        if (pGateway) stNetInfo.strGateway = pGateway->value();

        m_stSysCfg.mapNetInfo.insert(make_pair(stNetInfo.strName, stNetInfo));
    }

    // 查找控制网卡 数据网卡 IP地址
    auto it = m_stSysCfg.mapNetInfo.find(m_stSysCfg.strCtrlNet);
    if(it == m_stSysCfg.mapNetInfo.end()) {
        SAFE_FREE(pData);
        return false;
    }
    m_stSysCfg.strCtrlNetIP = it->second.strIP;

    it = m_stSysCfg.mapNetInfo.find(m_stSysCfg.strDataNet);
    if(it == m_stSysCfg.mapNetInfo.end())
        return false;
    m_stSysCfg.strDataNetIP = it->second.strIP;

    rapidxml::xml_node<char> * pPostMulticast = pSysConfig->first_node("PostMulticast");
    if (NULL == pPostMulticast) {
        log_error("cannot find node PostMulticast");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_attribute<char> * pBoardCheckIP = pIPInfo->first_attribute("BoardCheckIP");
    if (pBoardCheckIP) m_stSysCfg.strOutBoardCheckIP = pBoardCheckIP->value();

    rapidxml::xml_attribute<char> * pBoardCheckPort = pIPInfo->first_attribute("BoardCheckPort");
    if (pBoardCheckPort) m_stSysCfg.nOutBoardCheckPort = atoi(pBoardCheckPort->value());

    rapidxml::xml_attribute<char> * pResourceIP = pIPInfo->first_attribute("ResourceIP");
    if (pResourceIP) m_stSysCfg.strOutResourceIP = pResourceIP->value();

    rapidxml::xml_attribute<char> * pResourcePort = pIPInfo->first_attribute("ResourcePort");
    if (pResourcePort) m_stSysCfg.nOutResourcePort = atoi(pResourcePort->value());

    rapidxml::xml_node<char> * pResource = pSysConfig->first_node("Resource");
    if (NULL == pResource) {
        log_error("cannot find node Resource");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_attribute<char> * pSum = pResource->first_attribute("Sum");
    if (pSum) m_stSysCfg.nResourceSum = atoi(pSum->value());

    rapidxml::xml_attribute<char> * pMpeg2SDNeeds = pResource->first_attribute("Mpeg2SDNeeds");
    if (pMpeg2SDNeeds) m_stSysCfg.nResourceMpeg2SDNeeds = atoi(pMpeg2SDNeeds->value());

    rapidxml::xml_attribute<char> * pMpeg2HDNeeds = pResource->first_attribute("Mpeg2HDNeeds");
    if (pMpeg2HDNeeds) m_stSysCfg.nResourceMpeg2HDNeeds = atoi(pMpeg2HDNeeds->value());

    rapidxml::xml_attribute<char> * pH264SDNeeds = pResource->first_attribute("H264SDNeeds");
    if (pH264SDNeeds) m_stSysCfg.nResourceH264SDNeeds = atoi(pH264SDNeeds->value());

    rapidxml::xml_attribute<char> * pH264HDNeeds = pResource->first_attribute("H264HDNeeds");
    if (pH264HDNeeds) m_stSysCfg.nResourceH264HDNeeds = atoi(pH264HDNeeds->value());

    rapidxml::xml_attribute<char> * pH265SDNeeds = pResource->first_attribute("H265SDNeeds");
    if (pH264SDNeeds) m_stSysCfg.nResourceH265SDNeeds = atoi(pH264SDNeeds->value());

    rapidxml::xml_attribute<char> * pH265HDNeeds = pResource->first_attribute("H265HDNeeds");
    if (pH265HDNeeds) m_stSysCfg.nResourceH265HDNeeds = atoi(pH265HDNeeds->value());

    rapidxml::xml_attribute<char> * pAVSSDNeeds = pResource->first_attribute("AVSSDNeeds");
    if (pAVSSDNeeds) m_stSysCfg.nResourceAVSSDNeeds = atoi(pAVSSDNeeds->value());

    rapidxml::xml_attribute<char> * pAVSHDNeeds = pResource->first_attribute("AVSHDNeeds");
    if (pAVSHDNeeds) m_stSysCfg.nResourceAVSHDNeeds = atoi(pAVSHDNeeds->value());

    rapidxml::xml_node<char> * pBoardInfo = pSysConfig->first_node("BoardInfo");
    if (NULL == pBoardInfo) {
        log_error("cannot find node BoardInfo");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_attribute<char> * pOEM = pBoardInfo->first_attribute("OEM");
    if (pOEM) m_stSysCfg.strOEM = pOEM->value();

    rapidxml::xml_attribute<char> * pCardID = pBoardInfo->first_attribute("CardID");
    if (pCardID) m_stSysCfg.strCardID = pCardID->value();

    rapidxml::xml_node<char> * pTimeServerAddr = pSysConfig->first_node("TimeServerAddr");
    if (NULL == pTimeServerAddr) {
        log_error("cannot find node TimeServerAddr");
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_attribute<char> * pTimeServerIP = pBoardInfo->first_attribute("IP");
    if (pTimeServerIP) m_stSysCfg.strTimeServerIP = pTimeServerIP->value();

    rapidxml::xml_attribute<char> * pTimeServerPort = pBoardInfo->first_attribute("Port");
    if (pTimeServerPort) m_stSysCfg.nTimeServerPort = atoi(pTimeServerPort->value());

    SAFE_FREE(pData);
    return true;
}

bool CConfig::SetNetcardStatus(std::string strNetcardName, bool bActive)
{
    auto it = m_stSysCfg.mapNetInfo.find(strNetcardName);
    if(it != m_stSysCfg.mapNetInfo.end())
    {
        it->second.bActive = bActive;
    }

	return true;
}

bool CConfig::LoadChannel()
{
    std::string strPath = CFileEx::GetExeDirectory() + CFileEx::Separator() + SYS_CHANNEL_FILE;
    char * pData = NULL;

    if (!ReadFile(strPath.c_str(), pData)) return false;

    rapidxml::xml_document<> doc;
    try {
        // parse会改变 szXml 中的内容
        doc.parse<0>(pData);
    }
    catch (...)
    {
        printf("----err xml----\n%s\n----------\n", pData);
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_node<> * pChannelInfo = doc.first_node("ChannelInfo");
    if (NULL == pChannelInfo) {
        log_error("cannot find node ChannelInfo");
        SAFE_FREE(pData);
        return false;
    }

    for (rapidxml::xml_node<char> * pChannel = pChannelInfo->first_node("Channel");
         pChannel != NULL;
         pChannel = pChannel->next_sibling())
    {
        rapidxml::xml_attribute<char> * pID = pChannel->first_attribute("ID");
        if (NULL == pID) continue;

        int nIndex = atoi(pID->value());

        nIndex--;   // ChannelID是从1开始的

        SysChannel &stChannel = m_arrSysChannel[nIndex];
        stChannel.nIndex = nIndex;

        rapidxml::xml_attribute<char> * pTSOutAddr = pChannel->first_attribute("TSOutAddr");
        if (NULL == pTSOutAddr) continue;

        ParseAddrToIPPort(pTSOutAddr->value(), stChannel.stTSOutAddr.strIP, stChannel.stTSOutAddr.nPort);

        rapidxml::xml_attribute<char> * pTsOutSwicth = pChannel->first_attribute("TsOutSwicth");
        if (NULL == pTsOutSwicth) continue;

        stChannel.stTSOutAddr.nSwitch = atoi(pTsOutSwicth->value());
        stChannel.bStart = stChannel.stTSOutAddr.nSwitch == 1 ? true : false;

        rapidxml::xml_attribute<char> * pMonitorAddr = pChannel->first_attribute("MonitorAddr");
        if (NULL == pMonitorAddr) continue;

        ParseAddrToIPPort(pMonitorAddr->value(), stChannel.stMonitorOutAddr.strIP, stChannel.stMonitorOutAddr.nPort);

        rapidxml::xml_attribute<char> * pMonitorSwitch = pChannel->first_attribute("MonitorSwitch");
        if (NULL == pMonitorSwitch) continue;

        stChannel.stMonitorOutAddr.nSwitch = atoi(pMonitorSwitch->value());

        rapidxml::xml_node<char> * pIn = pChannel->first_node("In");
        if (NULL == pIn) continue;

        rapidxml::xml_attribute<char> * pTSInAddr = pIn->first_attribute("TSInAddr");
        if (NULL == pTSInAddr) continue;

        ParseAddrToIPPort(pTSInAddr->value(), stChannel.stTSInAddr.strIP, stChannel.stTSInAddr.nPort);

        rapidxml::xml_attribute<char> * pInVCode = pIn->first_attribute("VCode");
        if (NULL == pInVCode) continue;

        stChannel.stTSInParam.nVCode = atoi(pInVCode->value());

        rapidxml::xml_attribute<char> * pInACode = pIn->first_attribute("ACode");
        if (NULL == pInACode) continue;

        stChannel.stTSInParam.nACode = atoi(pInACode->value());

        rapidxml::xml_attribute<char> * pServiceID = pIn->first_attribute("ServiceID");
        if (NULL == pServiceID) continue;

        stChannel.stTSInParam.nServiceID = atoi(pServiceID->value());

        rapidxml::xml_attribute<char> * pVPID = pIn->first_attribute("VPID");
        if (NULL == pVPID) continue;

        stChannel.stTSInParam.nVPID = atoi(pVPID->value());

        rapidxml::xml_attribute<char> * pServiceType = pIn->first_attribute("ServiceType");
        if (NULL == pServiceType) continue;

        stChannel.stTSInParam.nServiceType = atoi(pServiceType->value());

        rapidxml::xml_attribute<char> * pAPIDList = pIn->first_attribute("APIDList");
        if (NULL == pAPIDList) continue;

        string strAPIDList = pAPIDList->value();
        int nStartPos = 0;
        int nEndPos = strAPIDList.find(",");
        while(string::npos != nEndPos)
        {
            int nAPID = atoi(strAPIDList.substr(nStartPos, nEndPos - nStartPos).c_str());
            stChannel.stTSInParam.vecAPID.push_back(nAPID);

            nStartPos = nEndPos + 1;
            nEndPos = strAPIDList.find(",", nStartPos);
        }
        int nAPID = atoi(strAPIDList.substr(nStartPos).c_str());
        stChannel.stTSInParam.vecAPID.push_back(nAPID);

        rapidxml::xml_node<char> * pOut = pChannel->first_node("Out");
        if (NULL == pOut) continue;

        rapidxml::xml_attribute<char> * pOutVCode = pOut->first_attribute("VCode");
        if (NULL == pOutVCode) continue;

        stChannel.stTSOutParam.nVCode = atoi(pOutVCode->value());

        rapidxml::xml_attribute<char> * pOutACode = pOut->first_attribute("ACode");
        if (NULL == pOutACode) continue;

        stChannel.stTSOutParam.nACode = atoi(pOutACode->value());

        rapidxml::xml_attribute<char> * pWidth = pOut->first_attribute("Width");
        if (NULL == pWidth) continue;

        stChannel.stTSOutParam.nWidth = atoi(pWidth->value());

        rapidxml::xml_attribute<char> * pHeight = pOut->first_attribute("Height");
        if (NULL == pHeight) continue;

        stChannel.stTSOutParam.nHeight = atoi(pHeight->value());

        rapidxml::xml_attribute<char> * pFrameRate = pOut->first_attribute("FrameRate");
        if (NULL == pFrameRate) continue;

        stChannel.stTSOutParam.nFrameRate = atoi(pFrameRate->value());

        rapidxml::xml_attribute<char> * pVRate = pOut->first_attribute("VRate");
        if (NULL == pVRate) continue;

        stChannel.stTSOutParam.nVRate = atoi(pVRate->value());

        rapidxml::xml_attribute<char> * pARate = pOut->first_attribute("ARate");
        if (NULL == pARate) continue;

        stChannel.stTSOutParam.nARate = atoi(pARate->value());

        rapidxml::xml_node<char> * pOSD = pChannel->first_node("OSD");
        if (NULL == pOSD) continue;

        rapidxml::xml_attribute<char> * pText = pOSD->first_attribute("Text");
        if (NULL == pText) continue;

        stChannel.stOSD.strText = pText->value();

        rapidxml::xml_attribute<char> * pFontSize = pOSD->first_attribute("FontSize");
        if (NULL == pFontSize) continue;

        stChannel.stOSD.nFontSize = atoi(pFontSize->value());

        rapidxml::xml_attribute<char> * pPosition = pOSD->first_attribute("Position");
        if (NULL == pPosition) continue;

        stChannel.stOSD.nPosition = atoi(pPosition->value());

        rapidxml::xml_attribute<char> * pAntiColor = pOSD->first_attribute("AntiColor");
        if (NULL == pAntiColor) continue;

        stChannel.stOSD.nAntiColor = atoi(pAntiColor->value());

        rapidxml::xml_attribute<char> * pAlign = pOSD->first_attribute("Align");
        if (NULL == pAlign) continue;

        stChannel.stOSD.nAlign = atoi(pAlign->value());

        rapidxml::xml_attribute<char> * pType = pOSD->first_attribute("Type");
        if (NULL == pType) continue;

        stChannel.stOSD.nType = atoi(pType->value());
    }

    SAFE_FREE(pData);
    return true;
}

bool CConfig::LoadMonitor()
{
    std::string strPath = CFileEx::GetExeDirectory() + CFileEx::Separator() + SYS_MONITOR_FILE;

    char * pData = NULL;

    if (!ReadFile(strPath.c_str(), pData)) return false;

    rapidxml::xml_document<> doc;
    try {
        // parse会改变 szXml 中的内容
        doc.parse<0>(pData);
    }
    catch (...)
    {
        printf("----err xml----\n%s\n----------\n", pData);
        SAFE_FREE(pData);
        return false;
    }

    rapidxml::xml_node<> * pMonitorInfo = doc.first_node("MonitorInfo");
    if (NULL == pMonitorInfo) {
        log_error("cannot find node MonitorInfo");
        SAFE_FREE(pData);
        return false;
    }

    for (rapidxml::xml_node<char> * pChannelMonitor = pMonitorInfo->first_node("ChannelMonitor");
         pChannelMonitor != NULL;
         pChannelMonitor = pChannelMonitor->next_sibling())
    {
        rapidxml::xml_attribute<char> * pID = pChannelMonitor->first_attribute("ChannelID");
        if (NULL == pID) continue;

        int nIndex = atoi(pID->value());

        nIndex--;   // ChannelID是从1开始的

        SysChannel &stChannel = m_arrSysChannel[nIndex];
        stChannel.nIndex = nIndex;

        rapidxml::xml_node<char> * pEnable = pChannelMonitor->first_node("Enable");
        if (NULL == pEnable) continue;

        rapidxml::xml_attribute<char> * pVMonitor = pEnable->first_attribute("VMonitor");
        if (NULL == pVMonitor) continue;

        stChannel.stMonitorEnable.nVideoMonitor = atoi(pVMonitor->value());

        rapidxml::xml_attribute<char> * pAMonitor = pEnable->first_attribute("AMonitor");
        if (NULL == pAMonitor) continue;

        stChannel.stMonitorEnable.nAudioMonitor = atoi(pVMonitor->value());

        rapidxml::xml_attribute<char> * pAMonitorChannel = pEnable->first_attribute("AMonitorChannel");
        if (NULL == pAMonitorChannel) continue;

        stChannel.stMonitorEnable.nAudioChannelMonitor = atoi(pAMonitorChannel->value());

        rapidxml::xml_node<char> * pVThrld = pChannelMonitor->first_node("VThrld");
        if (NULL == pVThrld) continue;

        rapidxml::xml_attribute<char> * pVLostTime = pVThrld->first_attribute("LostTime");
        if (NULL == pVLostTime) continue;

        stChannel.stThrldVideo.nLostTime = atoi(pVLostTime->value());

        rapidxml::xml_attribute<char> * pVSampleTime = pVThrld->first_attribute("SampleTime");
        if (NULL == pVSampleTime) continue;

        stChannel.stThrldVideo.nSampleTime = atoi(pVSampleTime->value());

        rapidxml::xml_attribute<char> * pVStillRatio = pVThrld->first_attribute("StillRatio");
        if (NULL == pVStillRatio) continue;

        stChannel.stThrldVideo.nStillRatio = atoi(pVStillRatio->value());

        rapidxml::xml_attribute<char> * pVStill = pVThrld->first_attribute("Still");
        if (NULL == pVStill) continue;

        stChannel.stThrldVideo.nStill = atoi(pVStill->value());

        rapidxml::xml_attribute<char> * pVBlack = pVThrld->first_attribute("Black");
        if (NULL == pVBlack) continue;

        stChannel.stThrldVideo.nBlack = atoi(pVBlack->value());

        rapidxml::xml_attribute<char> * pVXStart = pVThrld->first_attribute("XStart");
        if (NULL == pVXStart) continue;

        stChannel.stThrldVideo.nXStart = atoi(pVXStart->value());

        rapidxml::xml_attribute<char> * pVYStart = pVThrld->first_attribute("YStart");
        if (NULL == pVYStart) continue;

        stChannel.stThrldVideo.nYStart = atoi(pVYStart->value());

        rapidxml::xml_attribute<char> * pVXEnd = pVThrld->first_attribute("XEnd");
        if (NULL == pVXEnd) continue;

        stChannel.stThrldVideo.nXEnd = atoi(pVXEnd->value());

        rapidxml::xml_attribute<char> * pVYEnd = pVThrld->first_attribute("YEnd");
        if (NULL == pVYEnd) continue;

        stChannel.stThrldVideo.nYEnd = atoi(pVYEnd->value());

        rapidxml::xml_node<char> * pAThrld = pChannelMonitor->first_node("AThrld");
        if (NULL == pAThrld) continue;

        rapidxml::xml_attribute<char> * pALostTime = pAThrld->first_attribute("LostTime");
        if (NULL == pALostTime) continue;

        stChannel.stThrldAudio.nLostTime = atoi(pALostTime->value());

        rapidxml::xml_attribute<char> * pASampleTime = pAThrld->first_attribute("SampleTime");
        if (NULL == pASampleTime) continue;

        stChannel.stThrldAudio.nSampleTime = atoi(pASampleTime->value());

        rapidxml::xml_attribute<char> * pALost = pAThrld->first_attribute("Lost");
        if (NULL == pALost) continue;

        stChannel.stThrldAudio.nLost = atoi(pALost->value());

        rapidxml::xml_attribute<char> * pAHigher = pAThrld->first_attribute("Higher");
        if (NULL == pAHigher) continue;

        stChannel.stThrldAudio.nHigher = atoi(pAHigher->value());

        rapidxml::xml_attribute<char> * pALower = pAThrld->first_attribute("Lower");
        if (NULL == pALower) continue;

        stChannel.stThrldAudio.nLower = atoi(pALower->value());

        rapidxml::xml_attribute<char> * pALostLower = pAThrld->first_attribute("LostLower");
        if (NULL == pALostLower) continue;

        stChannel.stThrldAudio.nLostLower = atoi(pALostLower->value());

        rapidxml::xml_node<char> * pColorThrld = pChannelMonitor->first_node("ColorThrld");
        if (NULL == pColorThrld) continue;

        rapidxml::xml_attribute<char> * pMaxDiffValue = pColorThrld->first_attribute("MaxDiffValue");
        if (NULL == pMaxDiffValue) continue;

        stChannel.stThrldColorBar.nMaxDiffValue = atoi(pMaxDiffValue->value());

        rapidxml::xml_attribute<char> * pMinNum = pColorThrld->first_attribute("MinNum");
        if (NULL == pMinNum) continue;

        stChannel.stThrldColorBar.nMinNum = atoi(pMinNum->value());

        rapidxml::xml_attribute<char> * pColorXStart = pColorThrld->first_attribute("XStart");
        if (NULL == pColorXStart) continue;

        stChannel.stThrldColorBar.nXStart = atoi(pColorXStart->value());

        rapidxml::xml_attribute<char> * pColorYStart = pColorThrld->first_attribute("YStart");
        if (NULL == pColorYStart) continue;

        stChannel.stThrldColorBar.nYStart = atoi(pColorYStart->value());

        rapidxml::xml_attribute<char> * pColorXEnd = pColorThrld->first_attribute("XEnd");
        if (NULL == pColorXEnd) continue;

        stChannel.stThrldColorBar.nXEnd = atoi(pColorXEnd->value());

        rapidxml::xml_attribute<char> * pColorYEnd = pColorThrld->first_attribute("YEnd");
        if (NULL == pColorYEnd) continue;

        stChannel.stThrldColorBar.nYEnd = atoi(pColorYEnd->value());

        rapidxml::xml_node<char> * pRedThrld = pChannelMonitor->first_node("RedThrld");
        if (pRedThrld) GetMoreColorThrld(pRedThrld, stChannel.stThrldRed);

        rapidxml::xml_node<char> * pGreenThrld = pChannelMonitor->first_node("GreenThrld");
        if (pGreenThrld) GetMoreColorThrld(pGreenThrld, stChannel.stThrldGreen);

        rapidxml::xml_node<char> * pBlueThrld = pChannelMonitor->first_node("BlueThrld");
        if (pBlueThrld) GetMoreColorThrld(pBlueThrld, stChannel.stThrldBlue);

        rapidxml::xml_node<char> * pBlackThrld = pChannelMonitor->first_node("BlackThrld");
        if (pBlackThrld) GetMoreColorThrld(pBlackThrld, stChannel.stThrldBlack);

        rapidxml::xml_node<char> * pGrayThrld = pChannelMonitor->first_node("GrayThrld");
        if (pGrayThrld) GetMoreColorThrld(pGrayThrld, stChannel.stThrldGray);

        rapidxml::xml_node<char> * pWhiteThrld = pChannelMonitor->first_node("WhiteThrld");
        if (pWhiteThrld) GetMoreColorThrld(pWhiteThrld, stChannel.stThrldWhite);
    }

    return true;
}

void CConfig::GetMoreColorThrld(rapidxml::xml_node<char> * pThrld, MonitorColorFieldThrld &stThrld)
{
    rapidxml::xml_attribute<char> * pRedYUp = pThrld->first_attribute("RedYUp");
    if (pRedYUp) stThrld.nRedYUP = atoi(pRedYUp->value());

    rapidxml::xml_attribute<char> * pRedYDown = pThrld->first_attribute("RedYDown");
    if (pRedYDown) stThrld.nRedYDown = atoi(pRedYDown->value());

    rapidxml::xml_attribute<char> * pGreenUUp = pThrld->first_attribute("GreenUUp");
    if (pGreenUUp) stThrld.nGreenUUp = atoi(pGreenUUp->value());

    rapidxml::xml_attribute<char> * pGreenUDown = pThrld->first_attribute("GreenUDown");
    if (pGreenUDown) stThrld.nGreenUDown = atoi(pGreenUDown->value());

    rapidxml::xml_attribute<char> * pBlueVUp = pThrld->first_attribute("BlueVUp");
    if (pBlueVUp) stThrld.nBlueVUp = atoi(pBlueVUp->value());

    rapidxml::xml_attribute<char> * pBlueVDown = pThrld->first_attribute("BlueVDown");
    if (pBlueVDown) stThrld.nBlueVUp = atoi(pBlueVDown->value());
}

bool CConfig::ReadFile(const char * strFile, char * &pData)
{
    int nFd = open(strFile, O_RDONLY);
    if (-1 != nFd)
    {
        size_t nLen = lseek(nFd, 0L, SEEK_END);
        lseek(nFd, 0L, SEEK_SET);

        pData = (char *) malloc(nLen + 1);
        read(nFd, pData, nLen);
        pData[nLen] = '\0';

        close(nFd);
        nFd = -1;
        return true;
    }
    else
    {
        return false;
    }
}

bool CConfig::SaveConfig()
{
    CFileEx::SetCurDirectory(CFileEx::GetExeDirectory().c_str());
    CBHXml xml;

    xml.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");

    xml.AddElem("SysConfig");
    xml.IntoElem();

    // IPInfo
    xml.AddElem("IPInfo");
    xml.SetAttrib("CtrlNet", m_stSysCfg.strCtrlNet.c_str());
    xml.SetAttrib("CtrlPort", m_stSysCfg.nCtrlPort);
    xml.SetAttrib("DataNet", m_stSysCfg.strDataNet.c_str());
    xml.IntoElem();
    for(auto it = m_stSysCfg.mapNetInfo.begin(); it != m_stSysCfg.mapNetInfo.end(); ++it)
    {
        xml.AddElem("eth");
        xml.SetAttrib("Name", it->second.strName.c_str());
        xml.SetAttrib("IP", it->second.strIP.c_str());
        xml.SetAttrib("Netmask", it->second.strNetmask.c_str());
        xml.SetAttrib("Gateway", it->second.strGateway.c_str());
    }
    xml.OutOfElem();
    
    // PostMulticast
    xml.AddElem("PostMulticast");
    xml.SetAttrib("BoardCheckIP", m_stSysCfg.strOutBoardCheckIP.c_str());
    xml.SetAttrib("BoardCheckPort", m_stSysCfg.nOutBoardCheckPort);
    xml.SetAttrib("ResourceIP", m_stSysCfg.strOutResourceIP.c_str());
    xml.SetAttrib("ResourcePort", m_stSysCfg.nOutResourcePort);
    
    // Resource
    xml.AddElem("Resource");
    xml.SetAttrib("Sum", m_stSysCfg.nResourceSum);
    xml.SetAttrib("Mpeg2SDNeeds", m_stSysCfg.nResourceMpeg2SDNeeds);
    xml.SetAttrib("Mpeg2HDNeeds", m_stSysCfg.nResourceMpeg2HDNeeds);
    xml.SetAttrib("H264SDNeeds", m_stSysCfg.nResourceH264SDNeeds);
    xml.SetAttrib("H264HDNeeds", m_stSysCfg.nResourceH264HDNeeds);
    xml.SetAttrib("H265SDNeeds", m_stSysCfg.nResourceH265SDNeeds);
    xml.SetAttrib("H265HDNeeds", m_stSysCfg.nResourceH265HDNeeds);
    xml.SetAttrib("AVSSDNeeds", m_stSysCfg.nResourceAVSSDNeeds);
    xml.SetAttrib("AVSHDNeeds", m_stSysCfg.nResourceAVSHDNeeds);

    // BoardInfo
    xml.AddElem("BoardInfo");
    xml.SetAttrib("OEM", m_stSysCfg.strOEM.c_str());
    xml.SetAttrib("CardID", m_stSysCfg.strCardID.c_str());

    // TimeServerAddr
    xml.AddElem("TimeServerAddr");
    xml.SetAttrib("IP", m_stSysCfg.strTimeServerIP.c_str());
    xml.SetAttrib("Port", m_stSysCfg.nTimeServerPort);

    xml.OutOfElem();

    xml.Save(SYS_CONFIG_FILE);
    return true;
}

bool CConfig::SaveChannel()
{
    CFileEx::SetCurDirectory(CFileEx::GetExeDirectory().c_str());
    CBHXml xml;

    xml.SetDoc("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\r\n");

    xml.AddElem("ChannelInfo");
    xml.IntoElem();

    for(int i=0; i<CHANNEL_MAX_NUM; ++i)
    {
        SysChannel &stSysChannel = m_arrSysChannel[i];
        stSysChannel.nIndex = i+1;  // 通道号从1开始

        // Channel
        xml.AddElem("Channel");
        xml.SetAttrib("ID", stSysChannel.nIndex);
        string strTSOutAddr = stSysChannel.stTSOutAddr.strIP + ":" + to_string(stSysChannel.stTSOutAddr.nPort);
        xml.SetAttrib("TSOutAddr", strTSOutAddr.c_str());
        xml.SetAttrib("TsOutSwicth", stSysChannel.stTSOutAddr.nSwitch);
        string strMonitorOutAddr = stSysChannel.stMonitorOutAddr.strIP + ":" + to_string(stSysChannel.stMonitorOutAddr.nPort);
        xml.SetAttrib("MonitorAddr", strMonitorOutAddr.c_str());
        xml.SetAttrib("MonitorSwitch", stSysChannel.stMonitorOutAddr.nSwitch);

        xml.IntoElem();

        // In
        xml.AddElem("In");
        string strTSInAddr = stSysChannel.stTSInAddr.strIP + ":" + to_string(stSysChannel.stTSInAddr.nPort);
        xml.SetAttrib("TSInAddr", strTSInAddr.c_str());
        xml.SetAttrib("VCode", stSysChannel.stTSInParam.nVCode);
        xml.SetAttrib("ACode", stSysChannel.stTSInParam.nACode);
        xml.SetAttrib("ServiceID", stSysChannel.stTSInParam.nServiceID);
        xml.SetAttrib("VPID", stSysChannel.stTSInParam.nVPID);
        string strAPIDList("");
        if(!stSysChannel.stTSInParam.vecAPID.empty())
        {
            auto it = stSysChannel.stTSInParam.vecAPID.begin();
            while(1)
            {
                strAPIDList += to_string(*it);
                if(++it == stSysChannel.stTSInParam.vecAPID.end())
                    break;
                strAPIDList += ",";
            }
        }
        xml.SetAttrib("APIDList", strAPIDList.c_str());
        xml.SetAttrib("ServiceType", stSysChannel.stTSInParam.nServiceType);

        // Out
        xml.AddElem("Out");
        xml.SetAttrib("VCode", stSysChannel.stTSOutParam.nVCode);
        xml.SetAttrib("ACode", stSysChannel.stTSOutParam.nACode);
        xml.SetAttrib("Width", stSysChannel.stTSOutParam.nWidth);
        xml.SetAttrib("Height", stSysChannel.stTSOutParam.nHeight);
        xml.SetAttrib("FrameRate", stSysChannel.stTSOutParam.nFrameRate);
        xml.SetAttrib("VRate", stSysChannel.stTSOutParam.nVRate);
        xml.SetAttrib("ARate", stSysChannel.stTSOutParam.nARate);

        //OSD
        xml.AddElem("OSD");

		xml.SetAttrib("Text", stSysChannel.stOSD.strText.c_str());
		

        xml.SetAttrib("FontSize", stSysChannel.stOSD.nFontSize);
        xml.SetAttrib("Position", stSysChannel.stOSD.nPosition);
        xml.SetAttrib("AntiColor", stSysChannel.stOSD.nAntiColor);
        xml.SetAttrib("Align", stSysChannel.stOSD.nAlign);
        xml.SetAttrib("Type", stSysChannel.stOSD.nType);


        xml.OutOfElem();
    }

    xml.OutOfElem();

    xml.Save(SYS_CHANNEL_FILE);
    return true;
}

bool CConfig::Restore()
{
    // 1.Config文件恢复出厂设置 IP地址不恢复 厂商等信息不恢复
    m_stSysCfg.strOutBoardCheckIP = "224.1.1.2";
    m_stSysCfg.nOutBoardCheckPort = 5001;

    m_stSysCfg.strOutResourceIP = "224.1.1.5";
    m_stSysCfg.nOutResourcePort = 5001;

    SaveConfig();

    // 2.Channel文件恢复出厂设置
    SysChannel stDefaultChannel;
    for(int i=0; i<CHANNEL_MAX_NUM; ++i)
    {
        m_arrSysChannel[i] = stDefaultChannel;
        m_arrSysChannel[i].nIndex = i+1;    // 通道号从1开始
    }
    SaveChannel();

    return true;
}

int CConfig::UpdateSystemInfo()
{
	SystemInfo stInfo;
	memset(&stInfo, 0, sizeof(SystemInfo));

	char buffer[8192+1] = {0};
    int fd, len;
    char *p = NULL;
    int i;

    /* get load averages */
	{
		fd = open("/proc/loadavg", O_RDONLY);
		if (fd < 0) return fd;
		len = read(fd, buffer, sizeof(buffer)-1);
		close(fd);
		buffer[len] = '\0';

		stInfo.load_avg[0] = strtod(buffer, &p);
		stInfo.load_avg[1] = strtod(p, &p);
		stInfo.load_avg[2] = strtod(p, &p);
		p = skip_token(p);          /* skip running/tasks */
		p = skip_ws(p);
		if (*p)
			stInfo.last_pid = atoi(p);
		else
			stInfo.last_pid = -1;

		/* get the cpu time info */
		fd = open("/proc/stat", O_RDONLY);
		if (fd < 0) return fd;
		len = read(fd, buffer, sizeof(buffer)-1);
		close(fd);
		buffer[len] = '\0';

		/* summary */
		p = skip_token(buffer);         /* "cpu" */
		cp_time[0] = strtoul(p, &p, 0);
	    
		cp_time[1] = strtoul(p, &p, 0);
		cp_time[2] = strtoul(p, &p, 0);
		cp_time[3] = strtoul(p, &p, 0);

		cp_time[4] = strtoul(p, &p, 0);
		cp_time[5] = strtoul(p, &p, 0);
		cp_time[6] = strtoul(p, &p, 0);

		/* convert cp_time counts to percentages */
		percentages(NCPUSTATES, cpu_states[0], cp_time, cp_old[0], cp_diff);
		p = skip_line(p);

		i = 0;
		/* each core */
		while (*p && (i < MAXCORES) && (strncmp(p, "cpu", 3) == 0))
		{
			p = skip_token(p);         /* "cpux" */
			cp_time[0] = strtoul(p, &p, 0);

			cp_time[1] = strtoul(p, &p, 0);
			cp_time[2] = strtoul(p, &p, 0);
			cp_time[3] = strtoul(p, &p, 0);

			cp_time[4] = strtoul(p, &p, 0);
			cp_time[5] = strtoul(p, &p, 0);
			cp_time[6] = strtoul(p, &p, 0);

			/* convert cp_time counts to percentages */
			percentages(NCPUSTATES, cpu_states[i+1], cp_time, cp_old[i+1], cp_diff);

			p = skip_line(p);
			i++;
		}
		stInfo.core_cnt = i+1;

	}

    /* get system wide memory usage */
	{
		char *p = NULL;

		fd = open("/proc/meminfo", O_RDONLY);
		if (fd < 0) return fd;
		len = read(fd, buffer, sizeof(buffer)-1);
		close(fd);
		buffer[len] = '\0';

		/* be prepared for extra columns to appear be seeking
		   to ends of lines */
	    
		p = buffer;
		p = skip_token(p);
		memory_stats[0] = strtoul(p, &p, 10); /* total memory */
	    
		p = strchr(p, '\n');
		p = skip_token(p);
		memory_stats[1] = strtoul(p, &p, 10); /* free memory */
	    
		p = strchr(p, '\n');
		p = skip_token(p);					  /* skip memAvailable */

		p = strchr(p, '\n');
		p = skip_token(p);
		memory_stats[2] = strtoul(p, &p, 10); /* buffer memory */
	    
		p = strchr(p, '\n');
		p = skip_token(p);
		memory_stats[3] = strtoul(p, &p, 10); /* cached memory */
	    
		for(i = 0; i< 10 ;i++) 
		{
			p++;
			p = strchr(p, '\n');
		}
	    
		p = skip_token(p);
		memory_stats[4] = strtoul(p, &p, 10); /* total swap */
	    
		p = strchr(p, '\n');
		p = skip_token(p);
		memory_stats[5] = strtoul(p, &p, 10); /* free swap */
	}

    /* set arrays and strings */
    stInfo.cpustats = cpu_states;
    stInfo.memory = memory_stats;

	m_stSysCfg.nMemTotal = stInfo.memory[0];
	m_stSysCfg.nMemUse = stInfo.memory[0] - stInfo.memory[1] - stInfo.memory[2] - stInfo.memory[3];
	m_stSysCfg.nMemUsage = ((float)m_stSysCfg.nMemUse / (float)stInfo.memory[0]) * 10000;
	
	m_stSysCfg.nCpuSize = stInfo.core_cnt;
	//printf("nMemLeft:%d Mem:%d  ", nMemUsage, m_stSysCfg.nMemUsage);
	for(int i=0; i<stInfo.core_cnt; ++i)
	{
		m_stSysCfg.nCpuUsage[i] = (float)stInfo.cpustats[i][0];
		//printf("Cpu%d:%d", i-1, m_stSysCfg.nCpuUsage[i], stInfo.core_cnt);
	}
    //printf("\n");

    return 0;
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

