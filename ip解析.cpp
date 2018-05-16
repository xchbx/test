#include <stdio.h>
#include <string>
#define _CRT_SECURE_NO_WARNINGS
int main()
{
	/*char buf[10];

	buf[0] = 0x7E;
	buf[1] = 0x02;
	buf[2] = 0x01;
	buf[3] = 0x0D;

	printf("%d\n",sizeof(buf));*/

	std::string strIpAddr = "172.24.0.1";
	/*int nIndex = strIpAddr.find(".");
	std::string strTmp1 = strIpAddr.substr(0, nIndex);
	printf("%d\n", nIndex);
	printf("%s\n", strTmp1.c_str());

	int nTemp = strIpAddr.find(".");
	std::string strTmp = strIpAddr.substr(nIndex+1, nTemp-1);
	printf("%d\n", nIndex);
	printf("%s\n", strTmp.c_str());*/

	int i = 0;
	int j = 0;
	int sum = 0;

	int numIP[4];
	while (strIpAddr[i] != '\0')
	{
		while ((strIpAddr[i] != '.') && (strIpAddr[i] != '\0'))
		{
			sum = sum * 10 + strIpAddr[i] - '0';
			++i;
		}
		numIP[j++] = sum;
		sum = 0;
		if (strIpAddr[i] != '\0')
		{
			++i;
		}	
	}

	numIP[1] += 10;
	numIP[2] += 20;
	
	char IPAddrStr[20] = { 0 };
	sprintf(IPAddrStr, "%d.%d.%d.%d",(numIP[0]),(numIP[1]),(numIP[2]),(numIP[3]));

	strIpAddr = IPAddrStr;
	printf("%s\n", strIpAddr.c_str());
	return 0;
}
