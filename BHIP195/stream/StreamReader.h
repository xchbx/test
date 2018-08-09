#ifndef __STREAM_READER_H__
#define __STREAM_READER_H__

#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>

#include "UDPRecv.h"
#include "Common.h"
#include "ChannelStruct.h"
#include "RingBuf.h"
#include "RingBuffer.h"


struct ProgInfo
{
    INT nPmtPid;
    INT nPcrPid;
    INT nVPid;
    INT nVType;
    INT nAPid;
    INT nAType;

    ProgInfo(): nPmtPid(0), nPcrPid(0), nVPid(0), nVType(0), nAPid(0), nAType(0)
    {
    }
};

class CStreamReader
{
public:
    CStreamReader(int nID);
    virtual ~CStreamReader();

    virtual int Start(TSInAddr &stTSInAddr, TSInParam &stTSInParam);
    virtual void Stop();

    int GetRun() { return m_bRun; }

    TSInAddr & GetTSInAddr()    { return m_stTSInAddr; }
    TSInParam &GetTSInParam()   { return m_stTSInParam; }

    virtual bool ReadVFrame(unsigned char* &pBS, unsigned int & uLen, unsigned long long & ullPts, int &nScrambleFlag, bool& isGetESData);
    virtual bool ReadAFrame(unsigned char* &pBS, unsigned int & uLen, unsigned long long & ullPts);


private:
    static bool DataRecv(void* lp, unsigned char* pData, unsigned int uLen);


private:
    bool			m_bRun;

    bool			m_bVFindHead;
    bool            m_bAFindHead;

    unsigned int    m_uVDataSize;
    unsigned char * m_pVDataBuf;
    unsigned char * m_pVideoPesBuf;
    unsigned int    m_uVideoPesLen;

    unsigned int	m_nVWrPos;
    unsigned int	m_nVRdPos;

    unsigned int    m_uADataSize;
    unsigned char * m_pADataBuf;
    unsigned char * m_pAudioPesBuf;
    unsigned int    m_uAudioPesLen;

    unsigned int	m_nAWrPos;
    unsigned int	m_nARdPos;


private:
    int             m_nID;
    string          m_strLocalIP;
    TSInAddr        m_stTSInAddr;
    TSInParam       m_stTSInParam;

    int             m_nScrambleFlag;

    CUDPRecv	  * m_pUdpRecv;
};

#endif

