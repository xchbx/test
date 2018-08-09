#include "OSSocket.h"

#include <stdio.h>
#include <string.h>
#include <net/if.h>

/* IP地址的封装 */
OS_SockAddr::OS_SockAddr()
{
	iAddr.sin_family = AF_INET;
	iAddr.sin_addr.s_addr = 0;
	iAddr.sin_port = 0;
}

OS_SockAddr::OS_SockAddr(const char* ip, unsigned short port)
{
	iAddr.sin_family = AF_INET;
	iAddr.sin_addr.s_addr = inet_addr(ip);
	iAddr.sin_port = htons(port);
}

OS_SockAddr::OS_SockAddr(const char* ip)
{
	iAddr.sin_family = AF_INET;
	iAddr.sin_addr.s_addr = inet_addr(ip);
	iAddr.sin_port = 0;
}

OS_SockAddr::OS_SockAddr(unsigned short port)
{
	iAddr.sin_family = AF_INET;
	iAddr.sin_addr.s_addr = 0;
	iAddr.sin_port = htons(port);
}
OS_SockAddr::OS_SockAddr(sockaddr_in addr)
{
	iAddr = addr;
}

void OS_SockAddr::SetIp(const char* ip)
{
	iAddr.sin_addr.s_addr = inet_addr(ip);
}
void OS_SockAddr::SetIp(unsigned int ip)
{
	iAddr.sin_addr.s_addr = ip;
}
void OS_SockAddr::SetIp(sockaddr_in addr)
{
	iAddr = addr;
}
void OS_SockAddr::SetPort(unsigned short port)
{
	iAddr.sin_port = htons(port);
}

std::string OS_SockAddr::GetIp_str() const
{
	return std::string(inet_ntoa(iAddr.sin_addr));
}
unsigned int OS_SockAddr::GetIp_n() const
{
	return iAddr.sin_addr.s_addr;
}
unsigned short OS_SockAddr::GetPort() const
{
	return ntohs(iAddr.sin_port);
}


/* socket相关的工具函数 */
OS_Socket::OS_Socket()
:hSock(0)
{
}

int OS_Socket::SetOpt_RecvTimeout(int ms)
{
#ifdef _WIN32
	if(socket_setsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&ms,sizeof(int)) < 0) 
	{ 
		return  -1;
	} 
	return 0;

#else
	timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms % 1000 * 1000;

	if(socket_setsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv,sizeof(timeval)) < 0) 
	{ 
		return  -1;
	} 
	return 0;
#endif
}

int OS_Socket::SetOpt_SendTimeout(int ms)
{
#ifdef _WIN32
	if(socket_setsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&ms,sizeof(int)) < 0) 
	{ 
		return  -1;
	} 
	return 0;
#else
	timeval tv;
	tv.tv_sec = ms / 1000;
	tv.tv_usec = ms % 1000 * 1000;

	if(socket_setsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv,sizeof(timeval)) < 0) 
	{ 
		return  -1;
	} 
	return 0;
	
#endif
}

int OS_Socket::SetOpt_SendBuf(int nSize)
{
#ifdef _WIN32
	if(socket_setsockopt(hSock,SOL_SOCKET,SO_SNDBUF,(char*)&nSize,sizeof(int)) < 0) 
	{ 
		return  -1;
	} 
	return 0;
#else
	if(socket_setsockopt(hSock,SOL_SOCKET,SO_SNDBUF,(char*)&nSize,sizeof(nSize)) < 0) 
	{ 
		return  -1;
	} 

	//设置存活时间
	char curTTL = 16;  //组播16跳， 区域网
	setsockopt(hSock , IPPROTO_IP, IP_MULTICAST_TTL, &curTTL, sizeof(curTTL));

	curTTL = 64;		//单播64跳，地区网
	setsockopt(hSock , IPPROTO_IP, IP_TTL, &curTTL, sizeof(curTTL));
    return 0;
#endif
}

int OS_Socket::GetOpt_SendBuf()
{
	int nZero=0;

	socklen_t args_len = sizeof(int);
	if(socket_getsockopt(hSock,SOL_SOCKET,SO_SNDBUF,(char*)&nZero,&args_len) < 0) 
	{ 
		return  -1;
	} 

	return nZero;
}

int OS_Socket::GetOpt_RecvTimeout()
{
#ifdef _WIN32
	int args = 0;
	socklen_t args_len = sizeof(int);
	if(socket_getsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&args, &args_len) < 0) 
	{ 
		return  -1;
	} 

	return args;
#else
	timeval tv;
	socklen_t tv_len = sizeof(timeval);
	if(socket_getsockopt(hSock,SOL_SOCKET,SO_RCVTIMEO,(char*)&tv, &tv_len) < 0) 
	{ 
		return  -1;
	} 

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
	
}

int OS_Socket::GetOpt_SendTimeout()
{
#ifdef _WIN32
	int args = 0;
	socklen_t args_len = sizeof(int);
	if(socket_getsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&args, &args_len) < 0) 
	{ 
		return  -1;
	} 

	return args;
#else

	timeval tv;
	socklen_t tv_len = sizeof(timeval);
	if(socket_getsockopt(hSock,SOL_SOCKET,SO_SNDTIMEO,(char*)&tv, &tv_len) < 0) 
	{ 
		return  -1;
	} 

	return tv.tv_sec * 1000 + tv.tv_usec / 1000;
#endif
}

int OS_Socket::Ioctl_SetBlockedIo(bool blocked)
{
	unsigned   long   args =  blocked ? 0: 1;
	if( socket_ioctl(hSock,   FIONBIO,   (unsigned   long*)&args) < 0)
	{
		return -1;
	}
	return 0;
}

 int OS_Socket::SetOpt_ReuseAddr(bool reuse)
 {
	int opt = reuse ? 1: 0;
	socklen_t len = sizeof(opt);
	if (socket_setsockopt(hSock, SOL_SOCKET, SO_REUSEADDR,(char*)&opt,len) < 0)
	{
		return -1;
	}

	return 0;
 }

 int OS_Socket::SetOpt_ReusePort(bool reuse)
 {
     int opt = reuse ? 1: 0;
     socklen_t len = sizeof(opt);
     if (socket_setsockopt(hSock, SOL_SOCKET, SO_REUSEPORT,(char*)&opt,len) < 0)
     {
         return -1;
     }

     return 0;
 }

 int OS_Socket::GetPeerAddr(OS_SockAddr& addr) const
 {
	 socklen_t len = sizeof(addr);
	 if(getpeername(hSock, (sockaddr*)&addr, &len) < 0)
	 {
		 return -1;
	 }
	 return 0;
 }

 int OS_Socket::GetLocalAddr(OS_SockAddr& addr) const
 {
	 socklen_t len = sizeof(addr);
	 if(getsockname(hSock, (sockaddr*)&addr, &len) < 0) 
	 {
		 return -1;
	 }
	 return 0;
 }

/*************** TCP SOCKET ***************/
int OS_TcpSocket::Open(bool resue)
{
	if((int)hSock > 0)
	{
		return 0;
	}

	hSock = socket_open(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if((int)hSock < 0)
	{
		return -1;
	}

	SetOpt_ReuseAddr(resue);

	return 0;
}

int OS_TcpSocket::Open(const OS_SockAddr& addr, bool reuse)
{
	if(Open(reuse) < 0)
	{
		return -1;
	}
	if (socket_bind(hSock, (sockaddr*) &addr, sizeof(sockaddr_in)) < 0)
	{
		socket_close(hSock);
		hSock = -1;
		return -1;
	}

	return 0;
}

void OS_TcpSocket::Close()
{
	if((int)hSock >= 0)
	{
		shutdown(hSock, 2);
		socket_close(hSock);
		hSock = -1;
	}
}

// 服务器
int OS_TcpSocket::Listen(int backlog)
{
	if(socket_listen(hSock, backlog) < 0)
	{
		return -1;
	}

	return 0;
}

int OS_TcpSocket::Accept(OS_TcpSocket* peer, OS_SockAddr *addr/* = NULL*/)
{
	sockaddr_in iAddr;
	socklen_t iAddr_len = sizeof(sockaddr_in);
	socket_t peer_handle = socket_accept( hSock, (sockaddr*)&iAddr, &iAddr_len );
#ifdef _WIN32
	if (peer_handle == 0xffffffff)
	{
		return -1;
	}
#else
	if(peer_handle < 0)
	{
		return -1;
	}
#endif

	peer->hSock = peer_handle;
	if (NULL != addr)
	{
		addr->SetIp(iAddr);
	}
	return 0;
}

// 客户端
int OS_TcpSocket::Connect(const OS_SockAddr& addr)
{
	if(socket_connect(hSock, (sockaddr*) &addr, sizeof(sockaddr)) < 0)
	{
		return -1;
	}
	return 0;
}

// 发送接收
int OS_TcpSocket::Send(const void* buf, int len)
{
	// 发送
	int n = socket_send(hSock, (const char*) buf, len, 0);
	return n;
}

int OS_TcpSocket::Recv(void* buf, int len)
{
	// 接收
	int n = socket_recv(hSock, (char*) buf, len, 0);
	return n;
}

/* Udp Socket */

int OS_UdpSocket::Open(bool resue)
{
	hSock = socket(	AF_INET, SOCK_DGRAM, 0 );
	if((int)hSock < 0)
	{
		return -1;
	}

	SetOpt_ReuseAddr(resue);
	return 0;
}

int OS_UdpSocket::Open(const char* local_ip, int local_port)
{
	bool reuse = true;
	
	if(Open(reuse) < 0)
	{
		return -1;
	}

	// 绑定端口

	sockaddr_in addr;
	
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(local_ip);
	addr.sin_port = htons(local_port);
	
	if (socket_bind(hSock, (sockaddr*) &addr, sizeof(sockaddr)) < 0)
	{
		socket_close(hSock);
		hSock = -1;
		return -1;
	}

	return 0;
}

void OS_UdpSocket::Close()
{
	if((int)hSock >= 0)
	{
		shutdown(hSock, 2);
		socket_close(hSock);
		hSock = -1;
	}
}

int OS_UdpSocket::SendTo(const char * local_ip, const void* buf, int len, const OS_SockAddr& peer)
{
    if (hSock <= 0)
    {
        if (0 != Open(local_ip, 0))
        {
            printf("OS_UdpSocket::SendTo Open Error !\n");
            return -1;
        }

        SetOpt_SendBuf(1000000);
    }

	int  addr_len = sizeof(sockaddr_in);
	int n = socket_sendto(hSock, (const char*) buf, len, 0, (sockaddr*) &peer, addr_len);
	if(n < 0)
	{
        Close();
		return -1;
	}

	return n;
}

int OS_UdpSocket::RecvFrom( void* buf, int max_len, OS_SockAddr& peer)
{
	// 接收数据
	socklen_t  addr_len = sizeof(sockaddr_in);
	int n = socket_recvfrom(hSock, (char*) buf, max_len, 0, (sockaddr*) &peer, &addr_len);
	if(n < 0)
	{
		return -1;
	}

	return n;
}

int OS_McastSock::Open(const char* mcast_ip, int port, const char* local_ip, const char *local_ip_name /* = "eth0" */)
{
	hSock = socket(	AF_INET, SOCK_DGRAM, 0 );
	if((int)hSock < 0)
	{
		return -1;
	}

    int curFlags = fcntl(hSock, F_GETFL, 0);
    fcntl(hSock, F_SETFL, curFlags|O_NONBLOCK);

	// 一定需要
	if(SetOpt_ReuseAddr(true) < 0)
	{
		socket_close(hSock);
		hSock = -1;
		return -1;
	}

    if(SetOpt_ReusePort(true) < 0)
    {
        socket_close(hSock);
        hSock = -1;
        return -1;
    }

	// 绑定ip
	sockaddr_in addr;
	addr.sin_family = AF_INET;
#ifdef _WIN32
	addr.sin_addr.s_addr = inet_addr(local_ip);
#else
	//addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_addr.s_addr = inet_addr(mcast_ip);
#endif
	addr.sin_port = htons(port);
	if (socket_bind(hSock, (sockaddr*) &addr, sizeof(sockaddr)) < 0)
	{
		socket_close(hSock);
		hSock = -1;
		return -1;
	}

	// 设置接收缓冲
	int rcvbuf_size;
	int len = sizeof(int);

    rcvbuf_size  = 8196 * 1000;
    //rcvbuf_size = 1500 * 10;
	int iRet = setsockopt(hSock, SOL_SOCKET, SO_RCVBUF, (char *)&rcvbuf_size, len);
	if ( -1 == iRet)
	{
		socket_close(hSock);
		hSock = 0;
		return -1;
	}	

    //绑定本地网卡
    if(local_ip_name)
    {
        struct ifreq stIfreq;
        strcpy(stIfreq.ifr_name, local_ip_name);
        if(setsockopt(hSock, SOL_SOCKET, SO_BINDTODEVICE,  (char *)&stIfreq, sizeof(stIfreq)) < 0)
        {  
            socket_close(hSock);
            hSock = 0;
            return -1;
        }
    }

	// 注册组播
	m_McReq.imr_multiaddr.s_addr = inet_addr(mcast_ip); 
	m_McReq.imr_interface.s_addr = inet_addr(local_ip); 
	if (setsockopt(hSock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&m_McReq, sizeof(m_McReq)) < 0)
	{
		/*
		socket_close(hSock);
		hSock = -1;
		return -1;
		*/
	}

	return 0;
}

// socket关闭时自动退出组播组
void OS_McastSock::Close()
{
	if((int)hSock >= 0)
	{
		shutdown(hSock, 2);
		socket_close(hSock);
		hSock = -1;
	}
}

int OS_McastSock::SendTo(const void* buf, int len, const OS_SockAddr& peer)
{
	int  addr_len = sizeof(sockaddr_in);
	int n = socket_sendto(hSock, (const char*) buf, len, 0, (sockaddr*) &peer, addr_len);
	if(n < 0)
	{
		return -1;
	}

	return 0;
}

int OS_McastSock::RecvFrom( void* buf, int max_len, OS_SockAddr& peer)
{
	// 接收数据
	socklen_t  addr_len = sizeof(sockaddr_in);

 	/* set recvfrom from server timeout */
   	//struct timeval tv;
    //fd_set readfds;
   	//tv.tv_sec = 1;
   	//tv.tv_usec = 0;
   	//FD_ZERO(&readfds);
   	//FD_SET(hSock, &readfds);
   	//if (select(hSock+1,&readfds,NULL, NULL, &tv) > 0)
   	{
		int n = socket_recvfrom(hSock, (char*) buf, max_len, 0, (sockaddr*) &peer, &addr_len);
		if(n < 0)
		{
			return -1;
		}
		return n;
   	}
		
	return  -1;	
}

