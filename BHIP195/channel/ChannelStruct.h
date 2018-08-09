#ifndef CHANNEL_STRUCT_H_
#define CHANNEL_STRUCT_H_

#include "Common.h"


struct FrameBitStream
{
    unsigned char   * pData;
    unsigned int	  uLen;

    unsigned long long ullPts;

    FrameBitStream() : pData(NULL), uLen(0), ullPts(0) { }
    FrameBitStream( const FrameBitStream & node );
    FrameBitStream & operator = (const FrameBitStream & node);
};


// Monitor Out
struct MonitorOutAddr
{
    int     nSwitch;
    string  strIP;
    int     nPort;

    MonitorOutAddr(): nSwitch(0), strIP("232.0.0.100"), nPort(7001) 
    {}
};

// TS Out
struct TSOutAddr
{
    int     nSwitch;
    string  strIP;
    int     nPort;

    TSOutAddr(): nSwitch(0), strIP("233.0.0.100"), nPort(7001)
    {}
};

struct TSOutParam
{
    int     nVCode;
    int     nACode;
    int     nWidth;
    int     nHeight;
    int     nFrameRate;
    int     nVRate;
    int     nARate;

    int     nVStreamType;   // VCode转换
    int     nAStreamType;   // ACode转换

    TSOutParam(): nVCode(2), nACode(1), 
        nWidth(704), nHeight(576), nFrameRate(25), nVRate(1500), nARate(32),
        nVStreamType(0), nAStreamType(0)
    {}
};

// In
struct TSInParam
{
    int     nVCode;
    int     nACode;
    int     nServiceID;     // 节目号
    int     nVPID;
    vector<int> vecAPID;
    int     nServiceType;   // 节目类别(0-广播 1-电视)

    int     nVStreamType;   // VCode转换
    int     nAStreamType;   // ACode转换

    TSInParam();
    TSInParam(const TSInParam & node);
    TSInParam & operator = (const TSInParam & node);
};

struct TSInAddr
{
    string  strIP;
    int     nPort;

    TSInAddr(): strIP("231.0.0.100"), nPort(7001)
    {}


};

// OSD
struct OsdInfo
{
    string  strText;
    int     nFontSize;      // 字体大小
    int     nPosition;      // 位置信息 0-左上角 1-右上角 2-左下角 3-右下角
    int     nAntiColor;     // 是否反色 0-不启用发色 1-启动反色
    int     nAlign;         // 对齐方式 0-左对齐 1-居中 2-右对齐
    int     nType;          // 显示类型 1-第一行为text,第二行为时间信息  2-第一行为text,无时间信息  3-text和时间信息在一行显示

    OsdInfo(): strText("osd"), nFontSize(16), nPosition(0), nAntiColor(0), nAlign(0), nType(1) 
    {}
};


// Monitor Param Start
// Monitor Enable
struct MonitorEnable
{
    int     nVideoMonitor;        // 视频监测使能开关 比特0表示黑场监测,比特1表示静帧监测
    int     nAudioMonitor;        // 音频监测使能开关 比特0表示音频丢失,比特1表示音量过高,比特2表示音量过低
    int     nAudioChannelMonitor; // 音频通道监测使能开关 bit0-左声道,bit1-右声道

    MonitorEnable(): nVideoMonitor(7), nAudioMonitor(7), nAudioChannelMonitor(3) 
    {}
};

// Video Threshold
struct MonitorVThrld
{
    int     nLostTime;    
    int     nSampleTime;
    int     nStillRatio;
    int     nStill;
    int     nBlack;
    int     nXStart;
    int     nYStart;
    int     nXEnd;
    int     nYEnd;

    MonitorVThrld(): nLostTime(500), nSampleTime(100), nStillRatio(3), nStill(500), nBlack(150), nXStart(30), nYStart(30), nXEnd(322), nYEnd(258) 
    {}
};

// Audio Threshold
struct MonitorAThrld
{
    int     nLostTime;
    int     nSampleTime;
    int     nLost;
    int     nHigher;
    int     nLower;
    int     nLostLower;

    MonitorAThrld(): nLostTime(500), nSampleTime(500), nLost(3), nHigher(-3), nLower(-45), nLostLower(-48) 
    {}
};

// 彩条监测门限
struct MonitorColorBarThrld
{
    int     nMaxDiffValue;
    int     nMinNum;
    int     nXStart;
    int     nYStart;
    int     nXEnd;
    int     nYEnd;

    MonitorColorBarThrld(): nMaxDiffValue(150), nMinNum(4), nXStart(30), nYStart(30), nXEnd(322), nYEnd(258) 
    {}
};

// 彩场监测门限
struct MonitorColorFieldThrld
{
    int     nRedYUP;
    int     nRedYDown;
    int     nGreenUUp;
    int     nGreenUDown;
    int     nBlueVUp;
    int     nBlueVDown;

    MonitorColorFieldThrld(): nRedYUP(255), nRedYDown(80), nGreenUUp(16), nGreenUDown(0), nBlueVUp(16), nBlueVDown(0) 
    {}
};

#endif

