#include "NetcardInfo.h"

#include <sstream>
#include <iostream>
#include <fstream>
#include <fcntl.h>

#define		NET_CARD_NAME_SIZE	2048	//系统所有网卡的名称的最大长度


static CNetcardInfo *g_pNetcardInfo = NULL;
static const char * NETWORK_CONFIG_PATH = "/etc/sysconfig/network-scripts/ifcfg-";

static int calcNetmaskToPrefix(const char * strMask);

CNetcardInfo::CNetcardInfo()
: m_bInit(false)
{
	m_mapBandInfo.clear();
    m_mapCardInfo.clear();
    m_mapCardInfo_R.clear();

	InitAllNetCardInfo();
}

CNetcardInfo::~CNetcardInfo()
{
	m_mapBandInfo.clear();

    for (map<string, OneNetcard *>::iterator it = m_mapCardInfo.begin(); it != m_mapCardInfo.end(); it++)
    {
        OneNetcard * pCard = it->second;
        SAFE_DELETE(pCard);
    }
    m_mapCardInfo.clear();
    m_mapCardInfo_R.clear();
}

CNetcardInfo* CNetcardInfo::GetInstance() 
{
    if (g_pNetcardInfo == NULL)
    {
        g_pNetcardInfo = new CNetcardInfo();
    }

    return g_pNetcardInfo;
}

void CNetcardInfo::DelInstance()
{
    if (g_pNetcardInfo != NULL)
    {
        delete g_pNetcardInfo;
        g_pNetcardInfo = NULL;
    }
}

void CNetcardInfo::Init()
{
    InitNetcardName();
}

void CNetcardInfo::GetBandWidth(const string strIp, unsigned long long &ullRecv, unsigned long long &ullSend)
{
	GetCurBandWidth(strIp);

	map<string, NetCardBandInfo>::iterator it = m_mapBandInfo.find(strIp);
	if (it != m_mapBandInfo.end())
	{
		NetCardBandInfo & netInfo = it->second;

		ullRecv = netInfo.ullRecv - netInfo.ullLastRecv;
		ullSend = netInfo.ullSend - netInfo.ullLastSend;
	}
}

bool CNetcardInfo::InitNetcardName()
{
    struct ifaddrs * ifaddr = NULL;
    if (getifaddrs(&ifaddr) == -1) return false;

    struct ifaddrs * ifa = ifaddr;

	map<string, bool>       mapCardName;  
	mapCardName.clear();

    for ( ; ifa != NULL; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr == NULL) continue;

        int family = ifa->ifa_addr->sa_family;

        if (family == AF_INET) //|| family == AF_INET6)
        {
            char host[NI_MAXHOST];

#if 0
            int s = getnameinfo(ifa->ifa_addr,
                (family == AF_INET) ? sizeof(struct sockaddr_in) :
                sizeof(struct sockaddr_in6),
                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
#endif

            int s = getnameinfo(ifa->ifa_addr,
                sizeof(struct sockaddr_in), 
                host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (strncmp(host, "127.0.0.1", strlen("127.0.0.1")) == 0) continue;

			map<string, NetCardBandInfo>::iterator itBandInfo = m_mapBandInfo.find(host);
			if(itBandInfo == m_mapBandInfo.end())
			{
				NetCardBandInfo netInfo;
				netInfo.strCardName = ifa->ifa_name;
				m_mapBandInfo[host] = netInfo;
			}

            map<string, bool>::iterator it = mapCardName.find(ifa->ifa_name);
            if (it == mapCardName.end())
            {
                mapCardName[ifa->ifa_name] = true;
            }
        }
    }

	map<string, NetCardInfo>::iterator itMapCardInfo = m_mapCardInfo_R.begin();
	map<string, bool>::iterator itMapCardName;
	for( ; itMapCardInfo != m_mapCardInfo_R.end(); ++itMapCardInfo)
	{
		itMapCardName = mapCardName.find(itMapCardInfo->first);
		if(itMapCardName == mapCardName.end())
		{
			if(itMapCardInfo->second.bActive)
			{
				//printf("NetCardInfo:%s %d\n", itMapCardInfo->first.c_str(), false);
				itMapCardInfo->second.bActive = false;
				CConfig::GetInstance()->SetNetcardStatus(itMapCardInfo->first, false);
			}
		}
		else
		{
			if(!itMapCardInfo->second.bActive)
			{
				//printf("NetCardInfo:%s %d\n", itMapCardInfo->first.c_str(), true);
				itMapCardInfo->second.bActive = true;
				CConfig::GetInstance()->SetNetcardStatus(itMapCardInfo->first, true);
			}
		}
	}

    freeifaddrs(ifaddr);
    return true;
}

bool CNetcardInfo::InitAllNetCardInfo()
{
	bool bSuccess = true;

    // 增加以空格为分隔符 redhat系统网卡名与后面的bytes没有空格区分
    FILE *pShell = popen("cat /proc/net/dev | grep [0-9] | grep -v lo | awk -F':' '{print $1}' | sed s/[[:space:]]//g", "r");
	
	int nSize = NET_CARD_NAME_SIZE;
	char *pBuf = (char *)malloc(nSize);
	memset(pBuf, 0 ,nSize);
	fread(pBuf, nSize, 1, pShell);
//	printf("pBuf = %s\n",pBuf);
	if(pBuf[nSize-1] != '\0')
	{
		log_error("Netcard name too long\n");
		bSuccess = false;
	}

	int nIndex = 0;
	string strBuf(pBuf);
	string strTmp;

	while(strBuf.size())
	{
		nIndex = strBuf.find("\n");
		strTmp = strBuf.substr(0, nIndex);
		strBuf = strBuf.substr(nIndex+1);

		//printf("tmp: %s nIndex:%d\n", strTmp.c_str(), nIndex);

		map<string, OneNetcard *>::iterator it = m_mapCardInfo.find(strTmp);
		if (it == m_mapCardInfo.end())
		{
			OneNetcard * pNewCard = new OneNetcard();
			if (pNewCard)
			{
				m_mapCardInfo[strTmp] = pNewCard;
				pNewCard->InitCardInfo(strTmp.c_str());

				NetCardInfo nInfo;
				pNewCard->GetCardInfo(nInfo);
				m_mapCardInfo_R[strTmp] = nInfo;
			}
		}
	}

	if(pBuf != NULL)
	{
		free(pBuf);
		pBuf = NULL;
	}

	if(pShell != NULL)
	{
		pclose(pShell);
		pShell = NULL;
	}

	return bSuccess;
}


/*
// 无法获取到网关地址
bool CNetcardInfo::GetNetcardInfo()
{
	struct ifaddrs * ifaddr = NULL;
	if (getifaddrs(&ifaddr) == -1) return false;

    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };

    struct ifaddr * ifa = NULL;
	for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
	{
		if (ifa->ifa_addr == NULL) continue;

		int family = ifa->ifa_addr->sa_family;

		if (family == AF_INET) //|| family == AF_INET6)
		{
			char host[NI_MAXHOST];

#if 0
			int s = getnameinfo(ifa->ifa_addr,
				(family == AF_INET) ? sizeof(struct sockaddr_in) :
				sizeof(struct sockaddr_in6),
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);
#endif

			int s = getnameinfo(ifa->ifa_addr,
				sizeof(struct sockaddr_in), 
				host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

            if (strncmp(host, "127.0.0.1", strlen("127.0.0.1")) == 0) continue;

			NetCardBandInfo netInfo;
			netInfo.strCardName = ifa->ifa_name;
			m_mapBandInfo[host] = netInfo;

            {
                int rsock = socket(PF_INET, SOCK_DGRAM, 0);
                if (rsock < 0)
                {
                    freeifaddrs(ifaddr);
                    return false;
                }

                NetCardInfo netCardInfo;
                netCardInfo.strCardName = ifa->ifa_name;
                netCardInfo.strIpAddr = string(host, strlen(host));

                struct ifreq ifr;

                memset(&ifr, 0, sizeof(ifr));
                strncpy(ifr.ifr_name, ifa->ifa_name, IF_NAMESIZE);

                if ((ioctl(rsock, SIOCGIFHWADDR, &ifr)) == -1)
                {
                    log_error("Get HW Addr error!");
                    freeifaddrs(ifaddr);
                    close(rsock);
                }

                char mac_address[20];
                memset(mac_address, '\0', sizeof(mac_address));

                for (int i = 0, j = 0; i < 6; i++)
                {   
                    char const byte = ifr.ifr_hwaddr.sa_data[i];

                    mac_address[j] = hex_chars[ ( byte & 0xF0 ) >> 4 ]; j++;
                    mac_address[j] = hex_chars[ ( byte & 0x0F ) >> 0 ]; j++;
                    if (i < 5)
                    {   
                        mac_address[j] = ':'; j++;
                    }   
                }

                netCardInfo.strHwAddr = string(mac_address, strlen(mac_address));

                memset(&ifr, 0, sizeof(ifr));
                strncpy(ifr.ifr_name, ifa->ifa_name, IF_NAMESIZE);
                if ((ioctl(rsock, SIOCGIFNETMASK, &ifr)) == -1)
                {
                    log_error("Get HW Addr error!");
                    freeifaddrs(ifaddr);
                    close(rsock);
                }

                netCardInfo.strNetmask = string(inet_ntoa(((struct sockaddr_in *)(&ifr.ifr_netmask))->sin_addr));

                close(rsock);

                m_mapCardInfo[ifa->ifa_name] = netCardInfo;
            }
		}
	}

	freeifaddrs(ifaddr);
	return true;
}
*/

bool CNetcardInfo::GetCurBandWidth(const string strIp)
{
	map<string, NetCardBandInfo>::iterator it = m_mapBandInfo.find(strIp);
	if (it == m_mapBandInfo.end())
	{
		return false;
	}

	NetCardBandInfo & netInfo = it->second;
	string strCard = netInfo.strCardName;
	unsigned long long ullRecv = 0, ullSend = 0;

	string strFileRecv = "/sys/class/net/" + strCard + "/statistics/rx_bytes";
	string strFileSend = "/sys/class/net/" + strCard + "/statistics/tx_bytes";

	ifstream inRecv;
	inRecv.open(strFileRecv.c_str());
	ifstream inSend;
	inSend.open(strFileSend.c_str());

	string strRecvBytes, strSendBytes;

	getline(inRecv, strRecvBytes);
	getline(inSend, strSendBytes);

	stringstream streamRecv;
	streamRecv << strRecvBytes;
	streamRecv >> ullRecv;

	stringstream streamSend;
	streamSend << strSendBytes;
	streamSend >> ullSend;

	inRecv.close();
	inSend.close();

	if (netInfo.ullLastRecv || netInfo.ullLastSend)
	{
		netInfo.ullLastRecv = netInfo.ullRecv;
		netInfo.ullLastSend = netInfo.ullSend;
	}
	else
	{
		netInfo.ullLastRecv = ullRecv;
		netInfo.ullLastSend = ullSend;
	}

	netInfo.ullRecv = ullRecv;
	netInfo.ullSend = ullSend;

	return true;
}

bool CNetcardInfo::SetNetCardInfo(const NetCardInfo & nInfo)
{
    if (nInfo.strCardName.length() == 0) return false;

    map<string, OneNetcard *>::iterator it = m_mapCardInfo.find(nInfo.strCardName);
    if (it != m_mapCardInfo.end())
    {
        OneNetcard * pCard = it->second;
        pCard->SetCardInfo(nInfo);
        return true;
    }
    else
        return false;
}

bool CNetcardInfo::GetNetCardInfo(CBHXml &xml)
{
	map<string, NetCardInfo>::iterator it = m_mapCardInfo_R.begin();
	for ( ; it != m_mapCardInfo_R.end(); it++ )
	{
		NetCardInfo & nInfo = it->second;

		xml.AddElem("NetCard");

		xml.AddAttrib("Name", nInfo.strCardName.c_str());
		xml.AddAttrib("IP", nInfo.strIpAddr.c_str());
		xml.AddAttrib("Mask", nInfo.strNetmask.c_str());
		xml.AddAttrib("Gateway", nInfo.strGateway.c_str());
		xml.AddAttrib("MAC", nInfo.strHwAddr.c_str());
		xml.AddAttrib("Active", nInfo.bActive?"1":"0");
	}

	return true;
}

static char * skipLine(char * p)
{
    while (*p == '\n' || *p == '\r' )
    {
        p++;
    }
	if (*p == '#')
	{ 
		while (*p != '\n')		//   判断是否到行为
		{
			p++;
		}
	}
    return p;
}

bool OneNetcard::InitCardInfo(const char * name)
{
    m_strCardName = name;

    string strFile = string(NETWORK_CONFIG_PATH) + string(name);

    FILE * fp = fopen(strFile.c_str(), "rb");
    if (fp == NULL) return false;

    fseek(fp, 0L, SEEK_END);
    long lLen = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char * pOrg = (char *)malloc(lLen + 1);
    memset(pOrg, '\0', lLen + 1);
    fread(pOrg, 1, lLen, fp);
    fclose(fp);

    char * pData = pOrg;
    char * pStart = NULL;
    char * pEnd = NULL;
	
    while (*pData != '\0')
    {
        if (*pData == '#' || *pData == '\r' || *pData == '\n')
        {
            pData = skipLine(pData);			
            continue;
        }
        pStart = strchr(pData, '=');
        if (pStart)
        {
            string key = string(pData, pStart - pData);
            string k_value;

            char * pEnd_1 = strchr(pStart + 1, '\n');
            char * pEnd_2 = strchr(pStart + 1, '\r');

            pEnd = pEnd_1 ? pEnd_1 : pEnd_2;
            if (pEnd)
            {
                k_value = string(pStart + 1, pEnd - pStart - 1);
                pData = pEnd + 1;
            }
            else
            {
                break;
            }

            m_mapCardInfo[key] = k_value;
        }
        else
        {
            break;
        }
    }

    free(pOrg);

    return true;
}

bool OneNetcard::GetCardInfo(NetCardInfo & info)
{
    info.strCardName = m_strCardName;

    map<string, string>::iterator it = m_mapCardInfo.find("IPADDR");
    if (it != m_mapCardInfo.end())
        info.strIpAddr = it->second;

    it = m_mapCardInfo.find("NETMASK");
    if (it != m_mapCardInfo.end())
        info.strNetmask = it->second;
    else
    {
        it = m_mapCardInfo.find("PREFIX");
        if (it != m_mapCardInfo.end())
        {
            string strNetmask = it->second;
            int prefix = atoi(strNetmask.c_str());
            if (prefix > 0 && prefix <= 32)
            {
                sockaddr_in iAddr;
                iAddr.sin_addr.s_addr = htonl(~((1 << (32 - prefix)) - 1));
                info.strNetmask = string(inet_ntoa(iAddr.sin_addr));
            }
        }
    }

    it = m_mapCardInfo.find("GATEWAY");
    if (it != m_mapCardInfo.end())
        info.strGateway = it->second;

    it = m_mapCardInfo.find("HWADDR");
    if (it != m_mapCardInfo.end())
        info.strHwAddr = it->second;

    return true;
}

OneNetcard::OneNetcard()
{
    m_mapCardInfo.clear();
}

OneNetcard::~OneNetcard()
{
    m_mapCardInfo.clear();
}

int calcNetmaskToPrefix(const char * strMask)
{
    int prefix = -1;
    unsigned int mask[4];
    sscanf(strMask, "%u.%u.%u.%u", &mask[0], &mask[1], &mask[2], &mask[3]);

    for (int i = 0; i < 4; i++)
    {
        switch (mask[i])
        {
        case 0xFF:
            prefix = 8 + 8*i;
            continue;
        case 0xFE:
            prefix = 7 + 8*i;
            return prefix;
        case 0xFC:
            prefix = 6 + 8*i;
            return prefix;
        case 0xF8:
            prefix = 5 + 8*i;
            return prefix;
        case 0xF0:
            prefix = 4 + 8*i;
            return prefix;
        case 0xE0:
            prefix = 3 + 8*i;
            return prefix;
        case 0xC0:
            prefix = 2 + 8*i;
            return prefix;
        case 0x80:
            prefix = 1 + 8*i;
            return prefix;
        case 0x00:
            prefix = 8*i;
            return prefix;
        default:
            return prefix;
        }
    }

    return prefix;
}

bool OneNetcard::SetCardInfo(const NetCardInfo & info)
{
    if (info.strIpAddr.length())
    {
        m_mapCardInfo["IPADDR"] = info.strIpAddr;
    }

    if (info.strGateway.length())
    {
        m_mapCardInfo["GATEWAY"] = info.strGateway;
    }

    if (info.strNetmask.length())
    {
        map<string, string>::iterator it = m_mapCardInfo.find("NETMASK");
        if (it != m_mapCardInfo.end())
            m_mapCardInfo["NETMASK"] = info.strNetmask;
        else
        {
            it = m_mapCardInfo.find("PREFIX");
            if (it != m_mapCardInfo.end())
            {
                int prefix = calcNetmaskToPrefix(info.strNetmask.c_str());

                if (prefix != -1)
                {
                    char strPrefix[10] = { '\0' };
                    snprintf(strPrefix, sizeof(strPrefix), "%d", prefix);

                    m_mapCardInfo["PREFIX"] = strPrefix;
                }
            }
            else
            {
                m_mapCardInfo["NETMASK"] = info.strNetmask;
            }
        }
    }

    // Save network config
    return SaveCardInfo();
}

bool OneNetcard::SaveCardInfo()
{
    string strFile = string(NETWORK_CONFIG_PATH) + m_strCardName;
    FILE * fp = fopen(strFile.c_str(), "wb");
    if (fp == NULL) return false;

    map<string, string>::iterator it = m_mapCardInfo.begin();
    for ( ; it != m_mapCardInfo.end(); it++)
    {
        string key = it->first;
        string k_value = it->second;

        fwrite(key.c_str(), 1, key.length(), fp);
        fwrite("=", 1, 1, fp);
        fwrite(k_value.c_str(), 1, k_value.length(), fp);
        fwrite("\n", 1, 1, fp);
    }

    fclose(fp);
    return true;
}
