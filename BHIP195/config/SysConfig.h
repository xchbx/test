#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

#include <string>
#include <vector>
#include <map>

#include "ChannelStruct.h"
#include "OSSocket.h"
#include "OSMutex.h"

#include "rapidxml/rapidxml.hpp"
#include "rapidxml/rapidxml_utils.hpp"
#include "rapidxml/rapidxml_print.hpp"

using namespace std;


#define MAXCORES  128
#define MAXCARDS  32

#define NCPUSTATES 7
#define NMEMSTATS 6
#define NNETSTATS 4


struct SystemInfo
{
    int    last_pid;
    double load_avg[3];
    int    core_cnt; /* including summary "cpu" with index 0 */
    /* "user", 
     * "nice", 
     * "system", 
     * "idle" 
     **/
    int    (*cpustats)[NCPUSTATES];

    /* "total mem",
     * "free mem",
     * "buffers", 
     * "cached", 
     * "total swap", 
     * "free swap" 
     **/
    int    *memory;
};

struct NetInfo
{
    bool    bActive;
    string  strName;
    string  strIP;
    string  strNetmask;
    string  strGateway;
    string  strMAC;

    NetInfo(): bActive(true), strName("eth0"), strIP("192.168.15.124"), strNetmask("255.255.255.0"), strGateway("192.168.15.1"), strMAC("00:00:00:00:00:00")
    {
    }
};

// 系统配置文件
struct SysConfig
{
    string  strCtrlNet;
    int     nCtrlPort;
    string  strDataNet;

    string  strCtrlNetIP;
    string  strDataNetIP;

    // map<NetcardName, NetInfo>
    map<string, NetInfo> mapNetInfo;

    string  strOutBoardCheckIP;
    int     nOutBoardCheckPort;
    string  strOutResourceIP;
    int     nOutResourcePort;

    // 转码卡资源相关信息 SD标清 HD高清
    int     nResourceSum;
    int     nResourceMpeg2SDNeeds;
    int     nResourceMpeg2HDNeeds;

    int     nResourceH264SDNeeds;
    int     nResourceH264HDNeeds;

    int     nResourceH265SDNeeds;
    int     nResourceH265HDNeeds;

    int     nResourceAVSSDNeeds;
    int     nResourceAVSHDNeeds;

    // 板卡固件信息
    string  strOEM;     // 厂商信息
    string  strCardID;  // 板卡序列号

    // 校时服务器地址信息
    string  strTimeServerIP;
    int     nTimeServerPort;
	int		nEnable;            // ToDo

    // 系统状态
    int					nTemperature;		//温度
    int 				nCpuUsage[MAXCORES+1];//0 总CPU使用率  1-N CPU1-N使用率 范围：(0-10000)
    int					nCpuSize;			  //有效CPU数量

    int					nMemTotal;			//内存总量(KB)
    int					nMemUsage;			//内存使用率 范围：(0-10000)
    int					nMemUse;			//内存使用量(KB)

    SysConfig();
    SysConfig(const SysConfig &node);
    SysConfig & operator = (const SysConfig & node);
};

// 系统Channel资源配置文件
struct SysChannel
{
    bool        bStart;

    int         nIndex;

    TSInAddr    stTSInAddr;
    TSInParam   stTSInParam;
    TSOutAddr   stTSOutAddr;
    TSOutParam  stTSOutParam;

    MonitorOutAddr  stMonitorOutAddr;
    OsdInfo         stOSD;

    MonitorEnable           stMonitorEnable;
    MonitorVThrld           stThrldVideo;
    MonitorAThrld           stThrldAudio;
    MonitorColorBarThrld    stThrldColorBar;      // 彩条
    MonitorColorFieldThrld  stThrldRed;           // 红色彩场
    MonitorColorFieldThrld  stThrldGreen;         // 绿色彩场
    MonitorColorFieldThrld  stThrldBlue;          // 蓝色彩场
    MonitorColorFieldThrld  stThrldBlack;         // 黑色彩场
    MonitorColorFieldThrld  stThrldGray;          // 灰色彩场
    MonitorColorFieldThrld  stThrldWhite;         // 白色彩场

    
    SysChannel(): bStart(false), nIndex(0) {}
};


class CConfig
{
public:
	CConfig(void);
	~CConfig(void);

    static CConfig *GetInstance();
    static void DelInstance();

public:
	// 获取配置信息
	SysConfig &GetConfig() { return m_stSysCfg; }
	bool	LoadConfig();
	bool	SaveConfig();

    // 获取Channel配置信息
    SysChannel &GetChannel(int nIndex) { return m_arrSysChannel[nIndex]; }
    bool    LoadChannel();
    bool    LoadMonitor();
    bool    SaveChannel();

    bool    Restore(); //恢复出厂设置

private:
    void GetMoreColorThrld(rapidxml::xml_node<char> * pThrld, MonitorColorFieldThrld &stThrld);
    bool ReadFile(const char * strFile, char * &pData);

public:
	// 设置网卡激活状态 不保存到配置文件中
	bool	SetNetcardStatus(std::string strNetcardName, bool bActive);

	// 获取系统状态 CPU 内存使用情况
	int		UpdateSystemInfo();

protected:
	SysConfig	    m_stSysCfg;		                    // 系统配置
    SysChannel      m_arrSysChannel[CHANNEL_MAX_NUM];   // 系统通道配置        

	OS_Mutex	    m_mtxLock;		// 系统配置访问锁
};

#endif

