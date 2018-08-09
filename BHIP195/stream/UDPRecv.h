#ifndef __UDPRECV_H__
#define __UDPRECV_H__

#include <string>
#include <map>
#include <pthread.h>
#include "OSSocket.h"
#include "OSMutex.h"

using namespace std;

#define UDP_RECV_BUF_SIZE 1500 

typedef bool (*CBF_DataRecv)(void * lp, unsigned char * pData, unsigned int nLen);

class CUDPRecv
{
public:
    CUDPRecv(void);
    ~CUDPRecv(void);

public:
    bool StartRecv(std::string strIP, int nPort, std::string strBindIP);
    void StopRecv();
    int  GetSocketPort();

    bool GetRunStatus() { return m_bRun; }

    int ReadData();    
    
    void RegisterCallbackFunc(CBF_DataRecv pfnDataRecv, void *lp);

protected:
    //static void * TH_Work(void *lp);

public:
    std::string  m_strIP;
    int          m_nPort;
    std::string  m_strBindIP;
    OS_McastSock m_sockRecv;

    CBF_DataRecv m_pfnDataRecv;
    void*        m_lpContext;

    pthread_t	 m_thRecv;
    bool         m_bRun;
    bool         m_bReadFin;
    FILE *       m_pFile;

    unsigned char m_recv_buf[UDP_RECV_BUF_SIZE];
};

class CUdpRecvManager
{
public:
    CUdpRecvManager();
    ~CUdpRecvManager();

    bool StartUdpRecvManager(const char * local_ip);
    void StopUdpRecvManager();

    bool AddUdpRecv(int nID, CUDPRecv * pUdpRecv);
    bool DelUdpRecv(int nID);

    int GetUdpRecvCount() { return m_nUdpRecvCount; }

private:
    static void * TH_WORK(void *lp);
    void RecvData();

private:
    bool                        m_bRun;
    pthread_t                   m_thRecv;

    string                      m_strLocalIp;
    map<int, CUDPRecv *>        m_mapUdpRecv;
    OS_Mutex                    m_mutexUdpRecv;
    int                         m_nUdpRecvCount;

    int                         m_nEpollFd;
    int                         m_nMaxEpollEvents;
};

#endif
