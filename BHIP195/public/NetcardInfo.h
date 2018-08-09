#ifndef __NETCARD_INFO_H__
#define __NETCARD_INFO_H__

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <string>
#include <map>

#include "Common.h"
#include "SysConfig.h"
#include "BHXml.h"

using namespace std;

struct NetCardInfo
{
	bool   bActive;
    string strCardName;
    string strIpAddr;
    string strNetmask;
    string strGateway;
    string strHwAddr;

    NetCardInfo()
    {
		bActive = true;
        strCardName = strIpAddr = strNetmask = strGateway = strHwAddr = "";
    }

    NetCardInfo( const NetCardInfo & node )
    {
		bActive = node.bActive;
        strCardName = node.strCardName;
        strIpAddr = node.strIpAddr;
        strNetmask = node.strNetmask;
        strGateway = node.strGateway;
        strHwAddr = node.strHwAddr;
    }

    NetCardInfo & operator = (const NetCardInfo & node)
    {
        if (this != &node)
        {
			bActive = node.bActive;
            strCardName = node.strCardName;
            strIpAddr = node.strIpAddr;
            strNetmask = node.strNetmask;
            strGateway = node.strGateway;
            strHwAddr = node.strHwAddr;
        }

        return *this;
    }
};

struct NetCardBandInfo
{
	string strCardName;

	unsigned long long ullRecv;
	unsigned long long ullSend;

	unsigned long long ullLastRecv;
	unsigned long long ullLastSend;

	NetCardBandInfo() : strCardName(""), ullRecv(0), ullSend(0)
		, ullLastRecv(0), ullLastSend(0) { }

	NetCardBandInfo( const NetCardBandInfo & node )
	{
		strCardName = node.strCardName;

		ullRecv = node.ullRecv;
		ullSend = node.ullSend;

		ullLastRecv = node.ullLastRecv;
		ullLastSend = node.ullLastSend;
	}

	NetCardBandInfo & operator = (const NetCardBandInfo & node)
	{
		if (this != &node)
		{
			strCardName = node.strCardName;

			ullRecv = node.ullRecv;
			ullSend = node.ullSend;

			ullLastRecv = node.ullLastRecv;
			ullLastSend = node.ullLastSend;
		}

		return *this;
	}
};

class OneNetcard
{
public:
    OneNetcard();
    ~OneNetcard();

    bool SetCardInfo(const NetCardInfo & info);
    bool GetCardInfo(NetCardInfo & info);

    bool InitCardInfo(const char * name);

private:
    bool SaveCardInfo();

private:
    map<string, string> m_mapCardInfo;
    string              m_strCardName;
};

class CNetcardInfo
{
public:
	~CNetcardInfo();

    static CNetcardInfo* GetInstance();
    static void DelInstance();

    void Init();
	void GetBandWidth(const string strIp, unsigned long long &ullRecv, unsigned long long &ullSend);

    bool SetNetCardInfo(const NetCardInfo & nInfo);

	bool GetNetCardInfo(CBHXml &xml);

private:
    CNetcardInfo();
    static CNetcardInfo *m_pInstance;

	bool GetCurBandWidth(const string strIp);

    bool InitNetcardName();

	bool InitAllNetCardInfo();

private:
	map<string, NetCardBandInfo>	m_mapBandInfo;	// for带宽统计 key: ip
    map<string, OneNetcard *>       m_mapCardInfo;  // for网卡信息 key: card name
    map<string, NetCardInfo>        m_mapCardInfo_R;

	bool				m_bInit;
};

#endif

