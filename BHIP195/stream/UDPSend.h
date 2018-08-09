#ifndef __UDP_SEND_H__
#define __UDP_SEND_H__

#include <string>
#include <map>
#include <pthread.h>
#include "OSSocket.h"
#include "OSMutex.h"
#include "OSTime.h"

#include "RingBuf.h"
#include "RingBuffer.h"

using namespace std;

#define TS_PACK_LEN     188
#define SEND_DATA_LEN   1316  // 7*188
#define SEND_DATA_COUNT 1400*50

//数据传到StreamWrite中是 188*280000 大小
//发送端应稍大于这个值，因为发送端数据发送的慢

struct S_SendData
{
    unsigned char*  pData;
    int             nLen;
    bool            bVideo;

    S_SendData()
    {
        pData = NULL;
        nLen = 0;
        bVideo = false;
    }

    S_SendData(const S_SendData &node)
    {
        pData = node.pData;
        nLen = node.nLen;
        bVideo = node.bVideo;
    }

    S_SendData& operator = (const S_SendData &node)
    {
        if (this != &node)
        {	
            pData = node.pData;
            nLen = node.nLen;
            bVideo = node.bVideo;
        }

        return *this;
    }
};

class CUdpSend
{
public:
    CUdpSend();
    ~CUdpSend();

	void SetVRate(int vRate, int aRate);
    bool StartSend(string strIP, int nPort, string strBindIP);
    void StopSend();

    bool PushSendData(unsigned char * pData, int nLen, bool bVideo);
    int  SendData(OS_UdpSocket &sendSock);

    bool GetStop() { return m_bStop; }

private:
    int SendTo(unsigned char * pData, int nLen, OS_UdpSocket &sendSock);

public:
    string          m_strIP;
    int             m_nPort;
    int             m_nChannel;

private:
    bool            m_bStop;
    string          m_strBindIP;
    OS_SockAddr     m_addrSendTo;

	int				m_nVRate;
	int				m_nAudio;
	int				m_nSendDataCnt;
	int				m_nSendDataThres;

    OS_TimeVal      m_tmBegin;
    OS_TimeVal      m_tmCurrent;

    unsigned char   m_TSEmptyPtk[TS_PACK_LEN];
    unsigned char   m_UdpTsPack[SEND_DATA_LEN];

    CRingBuf        m_ringData;
    CRingBuffer<S_SendData, SEND_DATA_COUNT> m_dataRing;
};

#define MAX_UDP_SENDER  512 

struct S_UdpSend
{
    bool bUsed;
    CUdpSend * pUdpSend;

    S_UdpSend()
    {
        bUsed = false;
        pUdpSend = NULL;
    }

    S_UdpSend(const S_UdpSend &node)
    {
        bUsed = node.bUsed;
        pUdpSend = node.pUdpSend;
    }

    S_UdpSend& operator = (const S_UdpSend &node)
    {
        if (this != &node)
        {	
            bUsed = node.bUsed;
            pUdpSend = node.pUdpSend;
        }

        return *this;
    }
};

class CUdpSendManager
{
public:
    CUdpSendManager();
    ~CUdpSendManager();

    bool StartUdpSendManager(const char * local_ip);
    void StopUdpSendManager();

    bool AddUdpSend(CUdpSend * pUdpSend);
    bool DelUdpSend(int n);

    int GetUdpSendCount() { return m_nUdpSendCount; }

private:
    static void * TH_WORK(void *lp);
    void SendData();

private:
    bool                        m_bRun;
    pthread_t                   m_thSend;

	OS_UdpSocket			    m_send_sock;	//Manager中管理 Socket发送描述符，否则会出现接收端丢包问题

    string                      m_strLocalIp;
    OS_Mutex                    m_mutexUdpSend;
    S_UdpSend                   m_arrUdpSend[MAX_UDP_SENDER];
    int                         m_nUdpSendCount;
};

#endif