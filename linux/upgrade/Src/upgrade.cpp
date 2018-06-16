#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <linux/if.h>

#include <netinet/in.h>

#include <arpa/inet.h>   
#include <sys/socket.h> 
#include <netinet/in.h>   
#include <unistd.h>   
#include <fcntl.h>   
#include <string.h>		  
#include <stdlib.h>
#include <linux/if.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <arpa/inet.h>
#include <sys/shm.h> 
#include <errno.h>
#include <sys/types.h>
#include <signal.h>


typedef unsigned char UInt8;
typedef unsigned short UInt16;
typedef unsigned int UInt32;
typedef unsigned int Bool;
typedef int Int32;
typedef char Int8;
typedef unsigned char Uint8;
typedef int SOCKET;
typedef struct sockaddr * PSA;



#define FAST_SERVER     0
#define HYBRID_SERVER   0
#define NORMALNC_SERVER 0
#define NORMAL_SERVER   1


//返回信息的Buf及长度
static char NU_SendBuf[1024];

int iGetEthIp(int socket, const char *pEthName, struct sockaddr_in *pHost)
{
	struct ifreq req;
	bzero(&req, sizeof(struct ifreq));
	strcpy(req.ifr_name, pEthName);
	ioctl(socket, SIOCGIFADDR, &req);
	memcpy((char*)pHost, &req.ifr_addr, sizeof(struct sockaddr_in));
	return 0;
}

int BHUpgrade(const char * pLocalIP)
{
	SOCKET       stcp = -1;
	SOCKET       stcpactive = -1;
	SOCKET       stcpbusy = -1;

	struct       sockaddr_in sin1;
	struct       timeval timeout;        // Timeout struct for select
	socklen_t    size;
	fd_set       ibits, obits, xbits;

	unsigned int uRecvPort = 1001;
	int nRet = 0;

	// Create the main TCP listen socket
	stcp = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (stcp < 0)
	{
		printf("BHCWR410 Upgrade: socket error !\n");
		return 0;
	}

	struct       sockaddr_in host;
	iGetEthIp(stcp, "eth0", &host);


	// Set Port = 1001, leaving IP address = Any
	bzero(&sin1, sizeof(struct sockaddr_in));
	sin1.sin_family = AF_INET;
	sin1.sin_port   = htons(uRecvPort);
	sin1.sin_addr = host.sin_addr;


	// Bind socket
	if ((bind(stcp, (PSA) &sin1, sizeof(sin1))) < 0)
	{
		//printf("BHCWR410 Upgrade: bind error !\n");
		perror("bind");
		close(stcp);
		return 0;
	}

	// Start listening
	if (listen(stcp, 1) < 0)
	{
		printf("BHUpgrade: listen error !\n");
		close(stcp);
		return 0;
	}

	// Configure our timeout to be 60 seconds
	timeout.tv_sec  = 60;
	timeout.tv_usec = 0;

	// Run until task is destroyed by the system
	for(;;)
	{
		unsigned int ufileSize = 0;
		unsigned int ufileLength = 0;
		int nNum = 0;

		FILE * pFile = NULL;
		unsigned char bufptr[1024];

		// Clear the select flags
		FD_ZERO(&ibits);
		FD_ZERO(&obits);
		FD_ZERO(&xbits);

		// We examine the main TCP, UDP, and active TCP (if any)
		FD_SET(stcp, &ibits);

		// Wait for socket activity
		if (stcpactive < 0)
		{
			// Wait without timeout
			nRet = select( 4, &ibits, &obits, &xbits, 0 );
		}
		else
		{
			// Wait for set timeout - abort active connection on no activity

			FD_SET(stcpactive, &ibits);
			nRet = select( 4, &ibits, &obits, &xbits, &timeout );
			if( !nRet )
			{
				close(stcpactive);
				stcpactive = -1;
			}
		}

		if (nRet < 0 ) continue;

		// Check for a new TCP connection
		if( FD_ISSET(stcp, &ibits) )
		{
			printf("We have a new connection.\n");

			size = sizeof(sin1);
			stcpbusy = accept(stcp, (PSA)&sin1, &size);

			// If the active socket is free use it, else print out
			// a busy message
			if (stcpactive < 0)
			{
				stcpactive = stcpbusy;
				FD_SET(stcpactive, &ibits);
			}
			else
				close(stcpbusy);
		}

		//system("rm -fr bhca92p_install_pack");

		// Check for new data on active TCP connection
		while (stcpactive >= 0 && FD_ISSET(stcpactive, &ibits))
		{
			// There is data available on the active connection
			nNum = (int) recv(stcpactive, bufptr, 1024, 0);

			if (nNum > 0)
			{
				//fileLength protocol for .hex download
				if( (nNum == 6) && (*bufptr == 0x5a) && (*(bufptr+5) == 0x5a) && (NULL == pFile) )
				{
					ufileLength = bufptr[1];
					ufileLength = (ufileLength<<8) + (unsigned char)bufptr[2];
					ufileLength = (ufileLength<<8) + (unsigned char)bufptr[3];
					ufileLength = (ufileLength<<8) + (unsigned char)bufptr[4];

					NU_SendBuf[0] = 0x5a;
					NU_SendBuf[1] = 1;
					send(stcpactive, NU_SendBuf, 4, 0);

					pFile = fopen("/opt/upgrade/upgrade.tar.gz", "wb+");
					if (NULL == pFile)
					{
						printf("BHUpgrade: fopen error\n");
					}

					continue;
				}
				else if( NULL != pFile)
				{
					fwrite(bufptr, nNum, 1, pFile);
					ufileSize += nNum;

					NU_SendBuf[0] = 0x5a;
					NU_SendBuf[1] = 1;
					send(stcpactive, NU_SendBuf, 4, 0);

					if (ufileLength == ufileSize)
					{
						printf("BHUpgrade: Upgrade.tar.gz DownLoad complete!\n");

						fflush(pFile);
						fclose(pFile);
						pFile = NULL;

						system("/opt/upgrade/upgrade.sh");
					}
					else if(ufileLength < ufileSize)
					{
						//too long
						printf("File is too long, socket will be closed.\n");
						send(stcpactive, NU_SendBuf, 4, 0);
						close( stcpactive );
						stcpactive = -1;
						break;
					}
				}
				else
					break;
			}
			else
				break;
		}

		if (NULL != pFile)
		{
			fclose(pFile);
			pFile = NULL;
		}

		printf("Finish recv file, socket will be closed.\n");
		close( stcpactive );
		stcpactive = -1;
	}

	// We only get here on an error - close the sockets
	if( stcp >= 0 )
		close( stcp );

	return 0;
}

int main(int argc, char *argv[])
{
	if (0 == BHUpgrade(NULL))
	{
		printf("BHUpgrade error\n");
	}

	return 0;
}
