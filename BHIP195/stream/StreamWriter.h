#ifndef __STREAM_WRITER_H__
#define __STREAM_WRITER_H__

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <set>

#include "UDPSend.h"
#include "Common.h"
#include "ChannelStruct.h"
#include "ZThread.h"
#include "OSMutex.h"
#include "Thread.h"

#include "EncoderTS.h"
#include "ESParse.h"

using namespace VA;
using namespace std;

#define MAX_RTSP_SENDER  512

class CStreamWriter
{
public:
	CStreamWriter();
    virtual ~CStreamWriter();

	virtual bool Init(TSOutAddr &stTSOutAddr, TSOutParam &stTSOutParam) = 0;
	virtual void Stop() = 0 ;

	virtual mfxStatus WriteNextFrame(unsigned char* pData, int nLen, unsigned long long ullPts, int nStreamType, int nPid) = 0 ;
	virtual void      WriteAudioData(unsigned char* pData, int nLen, unsigned long long pts, int nStreamType, int nPid) = 0;

	virtual void	  SetAudioMode(bool bPureAudio) { m_bPureAudio = bPureAudio; }

public:
	int GetFailCode() { return m_nFailCode; }
    bool GetRunStatus() { return m_bRun; }


protected:
    unsigned int GetPayload(unsigned char* pInData, unsigned char* pOutData, unsigned int nOutLen);

protected:
    string            m_strLocalIP;
    TSOutAddr         m_stTSOutAddr;
    TSOutParam        m_stTSOutParam;

	int     		  m_nFailCode;
	bool			  m_bPureAudio;

protected:
    bool			 m_bAFindHead;

    unsigned char  * m_pAudioPesBuf;
    unsigned int     m_nAudioPesLen;
    unsigned char  * m_pAudioOutBuf;

    CESParse		 m_objEsParser;

	unsigned char  * m_pTSEmptyPtk;

	bool			 m_bRun;
};


class CUdpStreamWriter : public CStreamWriter
{
public :
	CUdpStreamWriter();
	virtual ~CUdpStreamWriter();

	virtual bool	  Init(TSOutAddr &stTSOutAddr, TSOutParam &stTSOutParam);
	virtual void	  Stop();
	
	virtual mfxStatus WriteNextFrame(unsigned char* pData, int nLen, unsigned long long ullPts, int nStreamType, int nPid);
	virtual void	  WriteAudioData(unsigned char* pData, int nLen, unsigned long long pts, int nStreamType, int nPid);

private:
	static void TSOutPut(void* pContext, unsigned char* pData, int nLen, bool bVideo);
	mfxStatus	ProcessVideo(unsigned char* Pdata, int len, unsigned long long uPts, int nStreamType, int nPid);

private:
	int			m_nVRate;
	int			m_nARate;

protected:
	CEncoderTs m_objEncoderTs;
    CUdpSend * m_pUdpSend;
};

#endif

