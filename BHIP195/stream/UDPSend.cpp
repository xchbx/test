#include "UDPSend.h"

#include <string.h>

map<string, CUdpSendManager *> g_mapUdpSendManager;

#define  SEND_TIME_DIFF			40

CUdpSend::CUdpSend()
: m_bStop(false)
, m_nChannel(-1)
, m_nVRate(0)
, m_nAudio(0)
, m_nSendDataCnt(0)
, m_nSendDataThres(0)
{
    m_ringData.Init(SEND_DATA_LEN * SEND_DATA_COUNT);

    memset(m_TSEmptyPtk, 0x00, TS_PACK_LEN);

    m_TSEmptyPtk[0] = 0x47;
    m_TSEmptyPtk[1] = 0x1F;
    m_TSEmptyPtk[2] = 0xFF;
    m_TSEmptyPtk[3] = 0x10;
}

void CUdpSend::SetVRate(int vRate, int aRate)
{
	int SendDateSize = 0;

	if ((m_nVRate != vRate) || (m_nAudio != aRate))
	{
		if (m_nVRate != vRate)
			m_nVRate = vRate;

		if (m_nAudio != aRate)
			m_nAudio = aRate;
		
        //计算间隔内发送带宽大小 kbps->B/ms
		m_nSendDataThres = (m_nVRate + m_nAudio)  * SEND_TIME_DIFF / 8 ;
	}
}

CUdpSend::~CUdpSend()
{

}

bool CUdpSend::StartSend(string strIP, int nPort, string strBindIP)
{
    m_strIP = strIP;
    m_nPort = nPort;
    m_strBindIP = strBindIP;

    m_bStop = false;

    OS_SockAddr addrTo(m_strIP.c_str(), m_nPort);
    m_addrSendTo = addrTo;

    OS_Time::GetLocalTime(&m_tmBegin);

    return true;
}

void CUdpSend::StopSend()
{
    m_bStop = true;

    //m_send_sock.Close();
}

static const int CONS_SEND_PACK_LEN = 7 * 188;

bool CUdpSend::PushSendData(unsigned char * pData, int nLen, bool bVideo)
{
    if (m_bStop) return false;

    bool bRet = true;
    unsigned char * pWrite = NULL;

    S_SendData stData;
    stData.bVideo = bVideo;

    if (nLen > 0)
    {
        int nNum = nLen / CONS_SEND_PACK_LEN;
        int nLeft = nLen % CONS_SEND_PACK_LEN;

        for (int n = 0; n < nNum; n++)
        {
            pWrite = m_ringData.Write(pData + n * CONS_SEND_PACK_LEN, CONS_SEND_PACK_LEN);
            if (pWrite)
            {
                stData.pData = pWrite;
                stData.nLen = CONS_SEND_PACK_LEN;

                if (! m_dataRing.Push(stData)) return false;
            }
            else
            {
                return false;
            }
        }

		if (nLeft != 0)
		{
			pWrite = m_ringData.Write(pData + nNum * CONS_SEND_PACK_LEN, nLeft);
			if (pWrite)
			{
				stData.pData = pWrite;
				stData.nLen = nLeft;
				if (!m_dataRing.Push(stData)) return false;
			}
		}
    }

    return true;
}


int CUdpSend::SendData(OS_UdpSocket &sendSock)
{
    int nBytes = 0;
    S_SendData stData;

    OS_Time::GetLocalTime(&m_tmCurrent);

    if (m_bStop) return nBytes;

	if (m_dataRing.GetData(stData))
	{
		if ((m_nSendDataCnt < m_nSendDataThres) && (m_tmBegin.MsTo(m_tmCurrent) < SEND_TIME_DIFF))
		{
			nBytes = sendSock.SendTo(m_strBindIP.c_str(), stData.pData, stData.nLen, m_addrSendTo);
            m_nSendDataCnt += nBytes;

			m_dataRing.PopData();
			m_ringData.SetRead(stData.pData);
		}

		if ((m_tmBegin.MsTo(m_tmCurrent)) > SEND_TIME_DIFF)
		{
			m_tmBegin = m_tmCurrent;
			m_nSendDataCnt = 0;
		}
	}

    return nBytes;
}

int CUdpSend::SendTo(unsigned char * pData, int nLen, OS_UdpSocket &sendSock)
{
    int nBytes = 0, nRet = 0; 

    int nNum = nLen / SEND_DATA_LEN;
    int nLeft = nLen % SEND_DATA_LEN;

    if (m_bStop) return nBytes;

    for (int n = 0; n < nNum; n++)
    {
        nRet = sendSock.SendTo(m_strBindIP.c_str(), pData + n * SEND_DATA_LEN, SEND_DATA_LEN, m_addrSendTo);
        
		if (nRet > 0)
        {
            nBytes += nRet;
        }
    }

    if (nLeft)
    { 
        nRet = sendSock.SendTo(m_strBindIP.c_str(), pData + nNum * SEND_DATA_LEN, nLeft, m_addrSendTo);
		if (nRet > 0)
        {
            nBytes += nRet;
        }
    }

    return nBytes;
}

CUdpSendManager::CUdpSendManager()
: m_bRun(false)
, m_thSend(-1)
, m_nUdpSendCount(0)
{
    m_mutexUdpSend.Init();
}

CUdpSendManager::~CUdpSendManager()
{
    StopUdpSendManager();
}

bool CUdpSendManager::StartUdpSendManager(const char * local_ip)
{
    m_strLocalIp = local_ip;

    m_bRun = true;
    int nErr = pthread_create(&m_thSend, NULL, TH_WORK, this);
    if (0 != nErr)
    {   
        m_bRun = false;
    }   

    return true;
}

void CUdpSendManager::StopUdpSendManager()
{
    if (!m_bRun)
    {
        return;
    }

    m_bRun = false;
    pthread_join(m_thSend, NULL);

    m_mutexUdpSend.Lock();
    m_mutexUdpSend.Unlock();
	
	m_send_sock.Close();
}

bool CUdpSendManager::AddUdpSend(CUdpSend * pUdpSend)
{
    bool bRet = true;

    m_mutexUdpSend.Lock();
    for (int i = 0; i < MAX_UDP_SENDER; i++)
    {
        if (m_arrUdpSend[i].bUsed == false)
        {
            pUdpSend->m_nChannel = i;
            m_arrUdpSend[i].pUdpSend = pUdpSend;
            m_arrUdpSend[i].bUsed = true;

            m_nUdpSendCount++;
            break;
        }
    }
    m_mutexUdpSend.Unlock();

    return bRet;
}

bool CUdpSendManager::DelUdpSend(int n)
{
    bool bRet = true;

    m_mutexUdpSend.Lock();
    m_arrUdpSend[n].pUdpSend = NULL;
    m_arrUdpSend[n].bUsed = false;

    if (m_nUdpSendCount > 0)
    {
        m_nUdpSendCount--;
    }
    m_mutexUdpSend.Unlock();

    return bRet;
}

void * CUdpSendManager::TH_WORK(void *lp)
{
    CUdpSendManager * pUdpSendManager = (CUdpSendManager *) lp;
    if (pUdpSendManager)
    {
        pUdpSendManager->SendData();
    }

    return NULL;
}

void CUdpSendManager::SendData()
{
    while (m_bRun)
    {
        int nBytes = 0;

        for(int i = 0; i < MAX_UDP_SENDER; i++)
        {
            if (m_arrUdpSend[i].bUsed)
            {
                CUdpSend * pUdpSend = m_arrUdpSend[i].pUdpSend;
                if (pUdpSend == NULL || pUdpSend->GetStop()) continue;

                nBytes += pUdpSend->SendData(m_send_sock);
            }
        }

        if (nBytes == 0)
        {
            usleep(100);
        }
    }
}
