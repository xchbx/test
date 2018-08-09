#include "UDPRecv.h"

#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

//#define TEST_INPUT_FILE

map<string, CUdpRecvManager *> g_mapUdpRecvManager;

CUDPRecv::CUDPRecv(void)
{
    m_strIP = "";
    m_strBindIP = "";
    m_nPort = 0;
    m_pfnDataRecv = NULL;
    m_lpContext = NULL;
    m_bRun = false;
    m_bReadFin = true;
    m_pFile = NULL;
}

CUDPRecv::~CUDPRecv(void)
{
    StopRecv();
}

void CUDPRecv::RegisterCallbackFunc(CBF_DataRecv pfnDataRecv, void *lp)
{
    m_pfnDataRecv = pfnDataRecv;
    m_lpContext = lp;
}

bool CUDPRecv::StartRecv(std::string strIP, int nPort, std::string strBindIP)
{
    if (m_bRun)
        return true;

    m_bRun = true;

    m_strIP = strIP;
    m_nPort = nPort;
    m_strBindIP = strBindIP;

#ifdef TEST_INPUT_FILE
    if ((m_pFile = fopen("heiping.ts", "rb")) == NULL)
    {
        m_bRun = false;
        return false;
    }
#else
    if (m_sockRecv.Open(strIP.c_str(), nPort, strBindIP.c_str()) != 0)
	{
		m_bRun = false;
        return false;
	}
#endif

#if 0
	int nErr = pthread_create(&m_thRecv, NULL, TH_Work, this);
    if (0 != nErr)
	{
		m_bRun = false;
        return false;
	}
#endif

    return true;
}

int CUDPRecv::ReadData()
{
    int nStatus = 0;
    OS_SockAddr clientsock;

    if (m_bRun == false)
    {
        return nStatus;
    }

    m_bReadFin = false;

#ifdef TEST_INPUT_FILE
    nStatus = fread(sbuf, 1, 65536, pThis->m_pFile);

#else
    nStatus = m_sockRecv.RecvFrom(m_recv_buf, UDP_RECV_BUF_SIZE, clientsock);
#endif

    if (nStatus > 0 && m_bRun && m_pfnDataRecv)
    {
        m_pfnDataRecv(m_lpContext, m_recv_buf, nStatus);
    }

    m_bReadFin = true;

    return nStatus;
}

void CUDPRecv::StopRecv()
{
    if (!m_bRun) return;
    m_bRun = false;
	//pthread_join(m_thRecv, NULL);

    while (m_bReadFin == false)
        usleep(200);

#ifdef TEST_INPUT_FILE
    if (m_pFile)
    {
        fclose(m_pFile);
        m_pFile = NULL;
    }  
#else
    m_sockRecv.Close();
#endif
}

int CUDPRecv::GetSocketPort()
{
    OS_SockAddr addr;
    m_sockRecv.GetLocalAddr(addr);

    return addr.GetPort();
}

CUdpRecvManager::CUdpRecvManager()
: m_bRun(false)
, m_thRecv(-1)
, m_nUdpRecvCount(0)
{
    m_mapUdpRecv.clear();
    m_mutexUdpRecv.Init();

    m_nMaxEpollEvents = 512;
    m_nEpollFd = epoll_create(m_nMaxEpollEvents + 1);
}

CUdpRecvManager::~CUdpRecvManager()
{
    StopUdpRecvManager();
}

void CUdpRecvManager::StopUdpRecvManager()
{
    if (!m_bRun)
    {
        return;
    }
    
    m_bRun = false;
    pthread_join(m_thRecv, NULL);

    close(m_nEpollFd);

    m_mutexUdpRecv.Lock();
    m_mapUdpRecv.clear();
    m_mutexUdpRecv.Unlock();
}

bool CUdpRecvManager::StartUdpRecvManager(const char * local_ip)
{
    m_strLocalIp = local_ip;

    m_bRun = true;
    int nErr = pthread_create(&m_thRecv, NULL, TH_WORK, this);
    if (0 != nErr)
    {   
        m_bRun = false;
    }   

    return true;
}

bool CUdpRecvManager::AddUdpRecv(int nID, CUDPRecv * pUdpRecv)
{
    bool bRet = true;

    m_mutexUdpRecv.Lock();
    map<int, CUDPRecv *>::iterator it = m_mapUdpRecv.find(nID);
    if (it == m_mapUdpRecv.end())
    {
        struct epoll_event ev;
        memset(&ev, 0, sizeof(struct epoll_event));

        //ev.data.fd = pUdpRecv->m_sockRecv.hSock;
        ev.data.ptr = (void *)pUdpRecv;
        ev.events = EPOLLIN;

        if (epoll_ctl(m_nEpollFd, EPOLL_CTL_ADD, pUdpRecv->m_sockRecv.hSock, &ev) == -1)
        {
            printf("[CUdpRecvManager::AddUdpRecv] for event EPOLLIN epoll_ctl EPOLL_CTL_ADD error\n");
        }

        m_mapUdpRecv[nID] = pUdpRecv;
        m_nUdpRecvCount++;
    }
    else
    {
        printf("[CUdpRecvManager::AddUdpRecv] Error: %s:%d already in the recv map\n", pUdpRecv->m_strIP.c_str(), pUdpRecv->m_nPort);
        bRet = false;
    }
    m_mutexUdpRecv.Unlock();

    return bRet;
}

bool CUdpRecvManager::DelUdpRecv(int nID)
{
    bool bRet = true;

    m_mutexUdpRecv.Lock();
    map<int, CUDPRecv *>::iterator it = m_mapUdpRecv.find(nID);
    if (it != m_mapUdpRecv.end())
    {
        CUDPRecv * pRecv = it->second;

        struct epoll_event ev;
        memset(&ev, 0, sizeof(struct epoll_event));

        epoll_ctl(m_nEpollFd, EPOLL_CTL_DEL, pRecv->m_sockRecv.hSock, &ev);

        m_mapUdpRecv.erase(it);

        if (m_nUdpRecvCount > 0)
        {
            m_nUdpRecvCount--;
        }
    }
    else
    {
        printf("[CUdpRecvManager::DelUdpRecv] Error: already del in the recv map\n");
        bRet = false;
    }
    m_mutexUdpRecv.Unlock();

    return bRet;
}

void * CUdpRecvManager::TH_WORK(void * lp)
{
    CUdpRecvManager * pUdpRecvManager = (CUdpRecvManager *) lp;
    if (pUdpRecvManager)
    {
        pUdpRecvManager->RecvData();
    }

    return NULL;
}

void CUdpRecvManager::RecvData()
{
    struct epoll_event events[m_nMaxEpollEvents];
    int epollTimeout = 10;

    while (m_bRun)
    {
        int ret = epoll_wait(m_nEpollFd, events, m_nMaxEpollEvents, epollTimeout);
        if (ret < 0) {
            printf("[CUdpRecvManager::RecvData] epoll_wait error\n");
            continue;
        }

        // TODO: ÓÅ»¯²éÕÒ
        for (int eventIdx = 0; eventIdx < ret; eventIdx++)
        {
            if (events[eventIdx].events & EPOLLIN)
            {
                CUDPRecv * pRecv = (CUDPRecv *) events[eventIdx].data.ptr;
                if (pRecv && pRecv->GetRunStatus())
                {
                    pRecv->ReadData();
                }
            }
        }
    }
}
