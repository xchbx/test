#include <sys/socket.h>
#include <netinet/init.h>
#include <arpa/inet.h>
#include  <stdio.h>
#include  <pthread.h>

void setBuf(char *buf)
{
	strcpy(buf,"111111111111111111111111");
}

void SendDataAppVersion()
{
	static char buf[100] = { 0 };
	static int len = 0;
	static int sock = 0;
	static struct sockaddr_in probe_addr;
	static socklen_t addr_len;


	if (0 == len)
	{
		sock = socket(AF_INET, SOCK_DGRAM, 0);
		if (sock < 0)
		{
			perror("error : socket create failed\n");
			return;
		}

		probe_addr.sin_family = AF_INET;
		probe_addr.sin_port = htons(8000);
		probe_addr.sin_addr.s_addr = inet_addr("244.2.2.2");
		bzero(&(probe_addr.sin_zero), 8);

		addr_len = sizeof(probe_addr);
	}
	
	setBuf(buf);

	if (len > 0)
	{
		perror("info : send AppVersion....\n");
		sendto(sock, buf, len, 0, (struct sockaddr *)&(probe_addr), addr_len);
	}
}

int main()
{
	pthread_t;
	int error=pthread_create(pthread_t,NULL,SendDataAppVersion,NULL);
	if(0!=error)
	{
		perror("pthread_create");
		exit(-1);
	}
	int errorJoin = pthread_join(thread,NULL);
	
	if(0!=errorJoin)
	{
		printf("pthread_join");
		exit(-1);
	}
}