#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define MAXLINE 1024
#define SERV_PORT 1001

#define TAR_FILE_NAME "./upgrade.tar.gz"

int getFileSize(char * strFileName)
{
	FILE * fp = fopen(strFileName, "r");
	if(fp==NULL)
	{
		perror("open file get size:");
		exit(-1);
	}
	fseek(fp, 0L, SEEK_END);
	int size = ftell(fp);
	fclose(fp);
	return size;
}

int main()
{
	struct sockaddr_in servAddr;
	char buf[MAXLINE];
	int sockfd, n;
	FILE *pFile = NULL;

	int receSize = 0;

	int writeLen = 0;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		perror("socket create error:");
		exit(-1);
	}

	bzero(&servAddr, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(SERV_PORT);
	servAddr.sin_addr.s_addr = inet_addr("192.168.161.130");

	int connVal = connect(sockfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
	if (connVal == -1)
	{
		perror("connect error:");
		close(sockfd);
		exit(-1);
	}

	int fileSize = getFileSize(TAR_FILE_NAME);

	printf("file size = %d\n", fileSize);

	if (fileSize > 0)
	{
		buf[0] = 0x5a;
		buf[1] = (unsigned char)(fileSize >> 24);
		buf[2] = (unsigned char)(fileSize >> 16);
		buf[3] = (unsigned char)(fileSize >> 8);
		buf[4] = (unsigned char)fileSize;
		buf[5] = 0x5a;
		

		writeLen = write(sockfd, buf, 6);		//发送协议头
		if (writeLen == -1)
		{
			perror("write error:");
			close(sockfd);
			exit(-1);
		}

		printf("send protocol head success\n");

		int nNum = read(sockfd, buf, MAXLINE);		//读服务器回复的协议头
		int readCount = 0;
		if ((nNum > 0) && (*buf == 0x5a))
		{
			printf("receive protocol head response success\n");
			pFile = fopen(TAR_FILE_NAME, "rb");
			if (pFile != NULL)
			{
				while (!feof(pFile))
				{
					readCount = fread(buf, 1, MAXLINE, pFile);	//前5个字节包含协议头 以及其他内容
					write(sockfd, buf, readCount);
					printf("send update date %d\n", readCount);

					nNum = read(sockfd, buf, MAXLINE);
					int receUpdateSize = 0;
					if ((nNum > 0) && (*buf == 0x5a))	//如果文件结束，则返回非0值，否则返回0
					{
						continue;
					}
	
					else
					{
						break;
					}
				}

			}
			fclose(pFile);
		}
	}
	close(sockfd);
	return 0;
}
