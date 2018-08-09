#include "StreamReader.h"
#include "AVCDemuxer.h"
#include "OSTime.h"
#include "TSParseCommon.h"
#include "SysConfig.h"

#include <math.h>
#include <iostream>

#define     MAX_A_PES_LEN       3200000     // 3M
#define     MAX_V_PES_LEN       10240000    // 10M
#define     PACKET_LEN          188
#define     GetPid(x)           ((x[1]&0x1f) << 8) + x[2]
#define     GetAdapCtrl(x)      ((x[3] >> 4) & 0x03)
#define     GetAdapLen(x)       x[4]
#define     GetStartFlag(x)     ((x[1] >> 6) & 0x01)


extern map<string, CUdpRecvManager *>   g_mapUdpRecvManager;

BOOL ParsePAT(unsigned char * pData, int & nPmt);
BOOL ParsePMT(unsigned char * pData, ProgInfo & stInfo);


CStreamReader::CStreamReader(int nID)
: m_bRun(false)
, m_bVFindHead(false)
, m_bAFindHead(false)
, m_uVDataSize(0)
, m_pVDataBuf(NULL)
, m_pVideoPesBuf(NULL)
, m_uVideoPesLen(0)
, m_nVWrPos(0)
, m_nVRdPos(0)
, m_uADataSize(0)
, m_pADataBuf(NULL)
, m_pAudioPesBuf(NULL)
, m_uAudioPesLen(0)
, m_nAWrPos(0)
, m_nARdPos(0)
, m_nID(nID)
, m_strLocalIP("")
, m_nScrambleFlag(0)
, m_pUdpRecv(NULL)
{
    m_uVDataSize = PACKET_LEN * 10000;
    m_pVDataBuf = (unsigned char *) malloc(m_uVDataSize);

    m_uADataSize = PACKET_LEN * 3000;
    m_pADataBuf = (unsigned char *) malloc(m_uADataSize);

    m_pVideoPesBuf = (unsigned char *) malloc(MAX_V_PES_LEN);
    m_pAudioPesBuf = (unsigned char *) malloc(MAX_A_PES_LEN);
}

CStreamReader::~CStreamReader()
{
    Stop();
}

void CStreamReader::Stop()
{
    if (!m_bRun) return;
    m_bRun = false;

    printf("----------------------- Del Stream Reader(%d), Receive IP %s:%d Local IP %s\n", m_nID, m_stTSInAddr.strIP.c_str(), m_stTSInAddr.nPort, m_strLocalIP.c_str());
    g_mapUdpRecvManager[m_strLocalIP]->DelUdpRecv(m_nID);

    SAFE_DELETE(m_pUdpRecv);

    SAFE_FREE(m_pVDataBuf);
    SAFE_FREE(m_pVideoPesBuf);

    SAFE_FREE(m_pADataBuf);
    SAFE_FREE(m_pAudioPesBuf);
}

int CStreamReader::Start(TSInAddr &stTSInAddr, TSInParam &stTSInParam)
{
    if (m_bRun) return -1;

    m_bVFindHead = false;
    m_bAFindHead = false;
    m_uVideoPesLen = 0;
    m_uAudioPesLen = 0;

    m_nAWrPos = 0;
    m_nARdPos = 0;

    m_nVWrPos = 0;
    m_nVRdPos = 0;

    m_strLocalIP = CConfig::GetInstance()->GetConfig().strDataNetIP;

    m_stTSInAddr = stTSInAddr;
    m_stTSInParam = stTSInParam;

    printf("+++++++++++++++++++++++ Add Stream Reader(%d), Receive IP %s:%d Local IP %s\n", m_nID, m_stTSInAddr.strIP.c_str(), m_stTSInAddr.nPort, m_strLocalIP.c_str());

    m_pUdpRecv = new CUDPRecv;
    m_pUdpRecv->RegisterCallbackFunc(DataRecv, this);
    if (NULL == m_pUdpRecv || !m_pUdpRecv->StartRecv(m_stTSInAddr.strIP, m_stTSInAddr.nPort, m_strLocalIP))
    {
        fprintf(stderr, "[CStreamReader::Start] Start CUDPRecv failed.\n");
        return -1;
    }

    g_mapUdpRecvManager[m_strLocalIP]->AddUdpRecv(m_nID, m_pUdpRecv);

    m_bRun = true;
    return 0;
}

bool CStreamReader::DataRecv(void* lp, unsigned char* pData, unsigned int uLen)
{
    CStreamReader* pThis = (CStreamReader*)lp;
    if (! (pThis && pThis->m_bRun)) return false;

    while (uLen >= PACKET_LEN)
    {
        int nPid = GetPid(pData);

        // 这里不用解析 直接使用配置中下发的参数即可
        if (nPid == pThis->m_stTSInParam.nVPID)
        {
            // 判断TS数据包是否加扰
            if(0x0 == GetScramble(pData))
            {
                if(1 == pThis->m_nScrambleFlag)
                {
                    printf("StreamID:%d PID:%d  ->  No Scramble\n", pThis->m_nID, nPid);    
                }
                pThis->m_nScrambleFlag = 0;
            }
            else
            {
                if(0 == pThis->m_nScrambleFlag)
                {
                    printf("StreamID:%d PID:%d  ->  Scramble:%d\n", pThis->m_nID, nPid, GetScramble(pData));    
                }
                pThis->m_nScrambleFlag = 1;
            }

            unsigned int uVWrPos = pThis->m_nVWrPos;
            unsigned int uVRdPos = pThis->m_nVRdPos;

            uVWrPos += PACKET_LEN;
            uVWrPos %= pThis->m_uVDataSize;

            // Is Full ?
            if (uVWrPos == uVRdPos)
            {
                log_info("UDP reader video buffer is full pid:%d nID:%d", nPid, pThis->m_nID);
                break;
            }

            if (pThis->m_bRun && pThis->m_pVDataBuf)
            {
                memcpy(pThis->m_pVDataBuf + pThis->m_nVWrPos, pData, PACKET_LEN);
                pThis->m_nVWrPos = uVWrPos;
            }
        }
        else if (nPid == pThis->m_stTSInParam.vecAPID[0])   // ToDo 支持第一个音频PID
        {
            unsigned int uAWrPos = pThis->m_nAWrPos;
            unsigned int uARdPos = pThis->m_nARdPos;

            uAWrPos += PACKET_LEN;
            uAWrPos %= pThis->m_uADataSize;

            // Is Full ?
            if (uAWrPos == uARdPos)
            {
                log_info("UDP reader audio buffer is full pid:%d nID:%d", nPid, pThis->m_nID);
                break;
            }

            if (pThis->m_bRun && pThis->m_pADataBuf)
            {
                memcpy(pThis->m_pADataBuf + pThis->m_nAWrPos, pData, PACKET_LEN);
                pThis->m_nAWrPos = uAWrPos;
            }
        }

        uLen -= PACKET_LEN;
        pData += PACKET_LEN;
    }

    return true;
}

bool CStreamReader::ReadVFrame(unsigned char* &pBS, unsigned int &uLen, unsigned long long & ullPts, int &nScrambleFlag,bool &isGetESData)
{
    // TS加扰标志
    nScrambleFlag = m_nScrambleFlag;

    while (m_bRun)
    {
        unsigned int uVWrPos = m_nVWrPos;
        unsigned int uVRdPos = m_nVRdPos;

        if (uVRdPos != uVWrPos)
        {
            unsigned int nPayLoadLen = 0;
            unsigned char* pData = m_pVDataBuf + uVRdPos;

            if (*pData != 0x47) 
            {
                uVRdPos += PACKET_LEN;
                uVRdPos %= m_uVDataSize;
                m_nVRdPos = uVRdPos;
                continue;
            }

            if ((false == m_bVFindHead) && (0 == GetStartFlag(pData)))
            {
                uVRdPos += PACKET_LEN;
                uVRdPos %= m_uVDataSize;
                m_nVRdPos = uVRdPos;
                continue;
            }

            m_bVFindHead = true;
			isGetESData = true;

            //判断开始标志位
            if (1 == GetStartFlag(pData) && (m_uVideoPesLen !=0)) 
            {
                uLen = ParsePesNotCpy(m_pVideoPesBuf, m_uVideoPesLen, &pBS, &ullPts, &m_nScrambleFlag);
                m_uVideoPesLen = 0;

                // TS+PES 加扰标志
                nScrambleFlag = m_nScrambleFlag;

                return uLen ? true : false;
            }

            nPayLoadLen = GetPayload(pData, m_pVideoPesBuf + m_uVideoPesLen, MAX_V_PES_LEN - m_uVideoPesLen);
            m_uVideoPesLen += nPayLoadLen;

            uVRdPos += PACKET_LEN;
            uVRdPos %= m_uVDataSize;
            m_nVRdPos = uVRdPos;
        }
        else
        {
            return false;
        }
    }

    return false;
}

bool CStreamReader::ReadAFrame(unsigned char* &pBS, unsigned int & uLen, unsigned long long & ullPts)
{
    while (m_bRun)
    {
        unsigned int uAWrPos = m_nAWrPos;
        unsigned int uARdPos = m_nARdPos;

        if (uARdPos != uAWrPos)
        {
            unsigned int nPayLoadLen = 0;
            unsigned char* pData = m_pADataBuf + uARdPos;

            if (*pData != 0x47)
            {
                uARdPos += PACKET_LEN;
                uARdPos %= m_uADataSize;
                m_nARdPos = uARdPos;
                continue;
            }

            if ((false == m_bAFindHead) && (0 == GetStartFlag(pData)))
            {
                uARdPos += PACKET_LEN;
                uARdPos %= m_uADataSize;
                m_nARdPos = uARdPos;
                continue;
            }

            m_bAFindHead = true;

            //判断开始标志位
            if (1 == GetStartFlag(pData) && (m_uAudioPesLen !=0)) 
            {
                unsigned long long pts = 0;
                uLen = ParsePesNotCpy(m_pAudioPesBuf, m_uAudioPesLen, &pBS, &ullPts);

                m_uAudioPesLen = 0;
                return uLen ? true : false;
            }

            nPayLoadLen = GetPayload(pData, m_pAudioPesBuf + m_uAudioPesLen, MAX_A_PES_LEN - m_uAudioPesLen);
            m_uAudioPesLen += nPayLoadLen;

            uARdPos += PACKET_LEN;
            uARdPos %= m_uADataSize;
            m_nARdPos = uARdPos;
        }
        else
        {
            return false;
        }
    }

    return false;
}

BOOL ParsePAT(unsigned char * pData, int & nPmt)
{
    BYTE byAdapCtrl = GetAdapCtrl(pData);
    UINT nPos = 4;

    if ((byAdapCtrl & 0x01) == 0)		//无有效负载
    {
        return FALSE;
    }

    if (byAdapCtrl & 0x02)			//有调整字段，跳过
    {
        UINT nAdapLen = pData[4];
        nPos += (nAdapLen + 1);
        nPos += 1;
    }
    else							//跳过Pointer_field
    {
        nPos += 1;
    }


    if (nPos > 180)					//剩余空间不足，PAT表可能跨两个TS包
    {
        return FALSE;
    }

    BYTE byTid = pData[nPos];
    nPos ++;
    UINT nSectionLen = ((pData[nPos] & 0x0f) << 8) | pData[nPos + 1];		//section长度
    nPos += 2;
    UINT nSectionStart = nPos;		//记录Section_length开始计数的字节位置

    if (byTid)						//Table_id不为0，错误
    {
        return FALSE;
    }

    nPos += 5;

    while (nPos - nSectionStart < nSectionLen - 4)
    {
        UINT nProgNum = (pData[nPos] << 8) | pData[nPos + 1];
        UINT nPid = ((pData[nPos + 2] & 0x1f) << 8) | pData[nPos + 3];

        nPos += 4;
        if (nProgNum)
        {
            nPmt = nPid;
            break;
        }
    }

    return TRUE;
}

BOOL ParsePMT(unsigned char *pData, ProgInfo & stInfo)
{
    BYTE byAdapCtrl = GetAdapCtrl(pData);
    UINT nPos = 4;

    if ((byAdapCtrl & 0x01) == 0)		//无有效负载
    {
        return FALSE;
    }

    if (byAdapCtrl & 0x02)			//有调整字段，跳过
    {
        UINT nAdapLen = pData[4];
        nPos += (nAdapLen + 1);
        nPos += 1;
    }
    else							//跳过Pointer_field
    {
        nPos += 1;
    }

    if (nPos > 170)					//剩余空间不足，PAT表可能跨两个TS包
    {
        return FALSE;
    }

    BYTE byTid = pData[nPos];
    nPos ++;
    UINT nSectionLen = ((pData[nPos] & 0x0f) << 8) | pData[nPos + 1];		//section长度
    nPos += 2;
    UINT nSectionStart = nPos;		//记录Section_length开始计数的字节位置

    if (byTid != 0x02)				//Table_id不为0x02，错误
    {
        return FALSE;
    }

    nPos += 5;

    //提取PCR_PID
    stInfo.nPcrPid = ((pData[nPos] & 0x1f) << 8) | pData[nPos + 1];
    nPos += 2;

    //提取Program_info_length
    UINT nProgInfoLen = ((pData[nPos] & 0x0f) << 8) | pData[nPos + 1];
    nPos += 2;
    nPos += nProgInfoLen;

    if (nPos > 180)
    {
        return FALSE;
    }

    //提取其他PID
    while (nPos - nSectionStart < nSectionLen - 4)
    {
        BYTE byStreamType = pData[nPos];
        nPos ++;
        UINT nPid = ((pData[nPos] & 0x1f) << 8) | pData[nPos + 1];
        nPos += 2;
        UINT nEsInfoLen = ((pData[nPos] & 0x0f) << 8) | pData[nPos + 1];
        nPos += 2;
        nPos += nEsInfoLen;

        switch (byStreamType)
        {
        case AV_CODEC_V_MPEG1:
        case AV_CODEC_V_MPEG2:
        case AV_CODEC_V_MPEG4:
        case AV_CODEC_V_H263:
        case AV_CODEC_V_H264:
        case AV_CODEC_V_HEVC:
        case AV_CODEC_V_AVS:
        case AV_CODEC_V_PRESENTATION_GRAPHICS:
        case AV_CODEC_V_INTERACTIVE_GRAPHICS:
        case AV_CODEC_V_SUBTITLE:
        case AV_CODEC_V_VC1:
            {
                stInfo.nVPid = nPid;
                stInfo.nVType = byStreamType;
            }
            break;
        case AV_CODEC_A_MPEG1:
        case AV_CODEC_A_MPEG2:
        case AV_CODEC_A_AC3_PRIVATE_STREAM:
        case AV_CODEC_A_AAC:
        case AV_CODEC_A_MPEG4:
        case AV_CODEC_A_AVS:
        case AV_CODEC_A_DRA:
        case AV_CODEC_A_LPCM:
        case AV_CODEC_A_AC3:
        case AV_CODEC_A_DTS:
        case AV_CODEC_A_AC3_TRUE_HD:
        case AV_CODEC_A_AC3_PLUS:
        case AV_CODEC_A_DTS_HD:
        case AV_CODEC_A_DTS_HD_MASTER:
        case AV_CODEC_A_SECONDARY_AC3_PLUS:
        case AV_CODEC_A_SECONDARY_DTS_HD:
            {
                // ToDo 多个音频PID的话 可以在这里使用数组进行添加
                stInfo.nAPid = nPid;
                stInfo.nAType = byStreamType;
            }
            break;
        default:
            {
                log_info("Other Data: Pid[%u] StreamType[%u]", nPid, byStreamType);
                break;
            }
        }
    }

    return TRUE;
}

