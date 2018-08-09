#include "StreamWriter.h"
#include "SysConfig.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <math.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <sys/prctl.h>
#include <sys/uio.h>

using namespace std;

#define     MAX_PES_LEN         10240000
#define		MAX_AUDIO_LEN		18000
#define     PACKET_LEN          188
#define     GetPid(x)           ((x[1]&0x1f) << 8) + x[2]
#define     GetAdapCtrl(x)      ((x[3] >> 4) & 0x03)
#define     GetAdapLen(x)       x[4]
#define     GetStartFlag(x)     ((x[1] >> 6) & 0x01)

#define     ADTS_HEADER_LEN     7


extern map<string, CUdpSendManager *> g_mapUdpSendManager;

CStreamWriter::CStreamWriter()
: m_strLocalIP("")
, m_nFailCode(0)
, m_bPureAudio(false)
, m_bAFindHead(false)
, m_pAudioPesBuf(NULL)
, m_nAudioPesLen(0)
, m_pAudioOutBuf(NULL)
{
    m_pAudioPesBuf = (unsigned char *) malloc(MAX_AUDIO_LEN);
    m_pAudioOutBuf = (unsigned char *) malloc(MAX_AUDIO_LEN);

	//TS 空包定义 PID=0x1FFF
	m_pTSEmptyPtk  = (unsigned char *) malloc(PACKET_LEN);
	memset(m_pTSEmptyPtk, 0x00, PACKET_LEN);

	m_pTSEmptyPtk[0] = 0x47;
	m_pTSEmptyPtk[1] = 0x1F;
	m_pTSEmptyPtk[2] = 0xFF;
	m_pTSEmptyPtk[3] = 0x10;

	m_bRun = false;
}

CStreamWriter::~CStreamWriter()
{
    SAFE_FREE(m_pAudioPesBuf);
    SAFE_FREE(m_pAudioOutBuf);
	SAFE_FREE(m_pTSEmptyPtk);
}

unsigned int CStreamWriter::GetPayload(unsigned char* pInData, unsigned char* pOutData, unsigned int nOutLen)
{
    unsigned int nPayloadLen = 0;

    unsigned char byAdapCtrl = GetAdapCtrl(pInData);
    unsigned int nPos = 4;

    //跳过调整字段
    if(byAdapCtrl & 0x02)
    {
        unsigned int nAdapLen = GetAdapLen(pInData);
        nPos += (nAdapLen + 1);
    }

    //判断有效负载长度是否正常
    if(nPos < PACKET_LEN)
    {   
        nPayloadLen = PACKET_LEN - nPos;
    }

    if(nOutLen < nPayloadLen)
    {
        return 0;
    }

    //拷贝数据
    memcpy(pOutData, pInData + nPos, nPayloadLen);

    return nPayloadLen;
}

CUdpStreamWriter::CUdpStreamWriter()
{
	m_nVRate = 0;
	m_nARate = 0;
    m_pUdpSend = new CUdpSend;
}

CUdpStreamWriter::~CUdpStreamWriter()
{
    Stop();
}

bool CUdpStreamWriter::Init(TSOutAddr &stTSOutAddr, TSOutParam &stTSOutParam)
{
    if (m_bRun) return true;
    
	if ((m_nVRate != stTSOutParam.nVRate) || (m_nARate != stTSOutParam.nARate))
    {
		m_nVRate = stTSOutParam.nVRate;
		m_nARate = stTSOutParam.nARate;
		m_pUdpSend->SetVRate(m_nVRate, m_nARate);
    }
	
    m_strLocalIP = CConfig::GetInstance()->GetConfig().strDataNetIP;
    m_stTSOutAddr = stTSOutAddr;
    m_stTSOutParam = stTSOutParam;

    m_pUdpSend->StartSend(m_stTSOutAddr.strIP, m_stTSOutAddr.nPort, m_strLocalIP);

    map<string, CUdpSendManager *>::iterator it = g_mapUdpSendManager.find(m_strLocalIP);
    if (it != g_mapUdpSendManager.end())
    {
        g_mapUdpSendManager[m_strLocalIP]->AddUdpSend(m_pUdpSend);
    }

	//注册ES转TS输出回调函数
    m_objEncoderTs.SetAudioMode(m_bPureAudio);
	m_objEncoderTs.RegOutPutCallBack(TSOutPut, this);
    m_objEncoderTs.Init(100, 50, 48, 49);    // ToDo
    printf("+++++++++++++++++++++++++++++ Add UDP Stream Writer, Send IP %s:%d Local IP %s\n",
        m_stTSOutAddr.strIP.c_str(), m_stTSOutAddr.nPort, m_strLocalIP.c_str());


    m_bRun = true;
	return true;
}

void CUdpStreamWriter::Stop()
{
    if(!m_bRun) return;
    m_bRun = false;

    m_pUdpSend->StopSend();
    map<string, CUdpSendManager *>::iterator it = g_mapUdpSendManager.find(m_strLocalIP);
    if (it != g_mapUdpSendManager.end())
    {
        g_mapUdpSendManager[m_strLocalIP]->DelUdpSend(m_pUdpSend->m_nChannel);
    }

    SAFE_DELETE(m_pUdpSend);
    printf("----------------------------- Del UDP Stream Writer\n");
}

#define     GetPid(x)           ((x[1]&0x1f) << 8) + x[2]

void CUdpStreamWriter::TSOutPut(void* pContext, unsigned char* pData, int nLen, bool bVideo)
{
	CUdpStreamWriter* pThis = (CUdpStreamWriter*)pContext;
	if (pThis == NULL || pThis->m_bRun == false || pData == NULL || nLen < 0) return;

	if (pThis->m_pUdpSend)
	{
		pThis->m_pUdpSend->PushSendData(pData, nLen, bVideo);
	}

    return;
}

mfxStatus CUdpStreamWriter::ProcessVideo(unsigned char* pData, int len, unsigned long long ullPts, int nStreamType, int nPid)
{
	m_objEncoderTs.EncodeEs(pData, len, ullPts, ullPts, nStreamType, nPid, true);
    return MFX_ERR_NONE;
}

void CUdpStreamWriter::WriteAudioData(unsigned char* pData, int nLen, unsigned long long pts, int nStreamType, int nPid)
{
    if (false == m_bRun) return;

    m_objEncoderTs.EncodeEs((unsigned char*)pData, nLen, pts, pts, nStreamType, nPid, false);

    return;

    if( (pData[0] == 0x47) && (nLen >= 188))
    {
        // ToDo Delete
        unsigned int nPayLoadLen = 0;

        if ((*pData != 0x47) || (nLen < PACKET_LEN))
            return;

        if (!GetStartFlag(pData) && (m_bAFindHead == false))
            return;

        m_bAFindHead = true;

        unsigned long long pts;

        if (GetStartFlag(pData) && (m_nAudioPesLen > 10))
        {
            int nAACLen = m_objEsParser.ParsePes(m_pAudioPesBuf, m_nAudioPesLen, m_pAudioOutBuf, &pts);
            m_objEncoderTs.EncodeEs(m_pAudioOutBuf, nAACLen, pts, pts, nStreamType, nPid, false);

            m_nAudioPesLen = 0;
        }

        if (m_nAudioPesLen > MAX_PES_LEN)
            return;	

        nPayLoadLen = GetPayload(pData, m_pAudioPesBuf + m_nAudioPesLen, MAX_PES_LEN);
        m_nAudioPesLen += nPayLoadLen;
    }
    else
    {
        m_objEncoderTs.EncodeEs((unsigned char*)pData, nLen, pts, pts, nStreamType, nPid, false);
    }
}

mfxStatus CUdpStreamWriter::WriteNextFrame(unsigned char* pData, int nLen, unsigned long long ullPts, int nStreamType, int nPid)
{
    if (false == m_bRun) return MFX_ERR_NONE;
    
	ProcessVideo(pData, nLen, ullPts, nStreamType, nPid);

	// mark that we don't need bit stream data any more
	//pMfxBitstream->DataLength = 0;

	return MFX_ERR_NONE;
}

