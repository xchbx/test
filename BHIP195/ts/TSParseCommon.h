#ifndef TS_PARSE_COMMON_H_
#define TS_PARSE_COMMON_H_

#define     GetPid(x)           ((x[1]&0x1f) << 8) + x[2]
#define     GetAdapCtrl(x)      ((x[3] >> 4) & 0x03)
#define     GetAdapLen(x)       x[4]
#define     GetStartFlag(x)     ((x[1] >> 6) & 0x01)
#define     GetScramble(x)      (x[3] >> 6)     // 获取TS头加扰标志


/*
	解析Pes数据 并将es数据带出
	pInData:	输入Pes数据
	nDataLen:	Pes数据长度
	pOut:		存放es数据的缓冲区
	pOutPts:	es数据pes值

	返回值:		0 解析失败 
				其他 es数据长度
*/
unsigned int ParsePes(unsigned char *pInData, unsigned int nDataLen, unsigned char* pOut, unsigned long long *pOutPts);

//与ParsePes函数区别：不带出es数据 只解析pes头部长度 放在pOffset中
unsigned int ParsePes(unsigned char *pInData, unsigned int nDataLen, unsigned long long *pOutPts, unsigned int *pOffset);

//与ParsePes函数区别：只将pOut指针指向es数据区 不进行数据拷贝
unsigned int ParsePesNotCpy(unsigned char *pInData, unsigned int nDataLen, unsigned char **pOut, unsigned long long *pOutPts, int *pScrambleFlag = 0);

/*
	获取Es数据Pts
	pInData:	输入Es数据
	nDataLen:	Es数据长度
	ipPts：		Es数据中的Pts值
*/
void GetPts(unsigned char *pInData, unsigned nDataLen, unsigned long long *ipPts);

/*
	获取TS有效负载(TS->Pes)
	pInData:	输入TS数据
	pOutData:	存放Pes数据缓冲区
	nOutLen:	pOutData长度

	返回值：	pes数据长度
*/
unsigned int GetPayload(unsigned char* pInData, unsigned char* pOutData, unsigned int nOutLen);

/*
	判断Es数据是否为I帧
	pEsData:	Es数据
	uLen:		Es数据长度

	返回值:		I帧 true 
				其他 false
*/
bool FindIDRFrame(unsigned char *pEsData, unsigned int uLen);

//从Es数据中寻找NAL单元
bool FindNAL(unsigned char *pData, unsigned int nEsLen, unsigned char &byState, unsigned int &nStartPos, unsigned int &nEndPos);

#endif

