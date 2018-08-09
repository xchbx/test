#pragma once

#include <stdio.h>
#include <string.h>
#include "OSTime.h"

namespace VA
{

#define TS_PACKET_LEN 188
#define POLY 0x04c11db7
#define _INIT	0
#define TS_PACKET_MAX_COUNT  280000 //7的倍数 需要 7*188向外发送数据

struct EncodeInfo
{
	int nPmtPid;
	int nProgNo;

    int nPcrPid;
	int nVideoPid;
	int nAudioPid;

	int nVideoStreamType;
	int nAudioStreamType;

	EncodeInfo()
	{
		nPmtPid = nProgNo = nVideoPid = nAudioPid = nVideoStreamType = nAudioStreamType = _INIT;
	}
};

//100ms
#define PAT_PMT_INTERVAL_TM		1000
#define PCR_INTERVAL_TM			1000

typedef void (*FuncOutPutTs)(void* pContext, unsigned char* pData, int nLen, bool bVideo);

class CEncoderTs
{
public:
	CEncoderTs();
	virtual ~CEncoderTs();

public:
    void Init(int nPmtPid, int nPcrPid, int nVPid, int nAPid);

	void EncodeEs(unsigned char* pData, int nLen, unsigned long long ullDTS, unsigned long long llPTS, int nStreamType, int nPid, bool bVideo = true);
	void InputTs(unsigned char* pData, int nLen, int nStreamType, int nPid, bool bIsAudio = true);//bIsAudio保留

	void RegOutPutCallBack(FuncOutPutTs pFnOutPut, void* pContext);

	void SetAudioMode(bool bPureAudio) { m_bPureAudio = bPureAudio; }

private:
	void GenPAT();
	void GenPMT();
	void GenPCR(unsigned long long ullPCR);

	void AddPCR(unsigned char *&pTsData,  unsigned long long ullPTS, int &nTsCount);
	void AddPSI(unsigned char *&pTsData,  unsigned long long ullPTS, int &nTsCount);

	void InitCRC32Table();
	unsigned int CheckCRC(unsigned char* pBegin, unsigned char* pEnd);

	bool Demux(unsigned char* buf, unsigned char*& data, bool& is_start, unsigned long long& i_dts, unsigned long long& i_pts);

private:
	EncodeInfo m_ndEncodeInfo;

	unsigned char m_bytePAT[TS_PACKET_LEN];
	unsigned char m_bytePMT[TS_PACKET_LEN];
	unsigned char m_bytePCR[TS_PACKET_LEN];
	//unsigned char m_byteVideo[TS_PACKET_LEN * TS_PACKET_MAX_COUNT];
	unsigned char * m_byteVideo;

	FuncOutPutTs	m_pFnOutPut;
	void*			m_pContext;
	
	unsigned int m_nCrc32Table[256];//CRC32校验

	unsigned long long m_ullLastSendPSITime;
	unsigned long long m_ullLastSendPcrTime;

	unsigned char m_uPatCC;
	unsigned char m_uPmtCC;
	unsigned char m_uVideoCC;
	unsigned char m_uAudioCC;
	unsigned char m_uPcrCC;
	
	FILE* m_pMediaTSFile;

	bool m_bPMTVersionChange;
	char m_bytePMTVersion;

	bool m_bPureAudio;

	bool m_bHavePSI;
	bool m_bHavePCR;

	int	 m_nAudioDataLen;

	bool m_bPcrVPidEqual;
};

}
