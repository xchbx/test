#include "TSParseCommon.h"
#include "Common.h"

#define STREAM_ID_MAP           0xBC
#define STREAM_ID_1             0xBD
#define STREAM_ID_PADDING       0xBE
#define STREAM_ID_2             0xBF
#define STREAM_ID_ECM           0xF0
#define STREAM_ID_EMM           0xF1
#define STREAM_ID_DSMCC         0xF2
#define STREAM_ID_ISO13522      0xF3
#define STREAM_ID_DIR           0xFF

enum {
	NAL_NULL=0,
	NAL_SLICE,
	NAL_DPA,
	NAL_DPB,
	NAL_DPC,
	NAL_IDR_SLICE,
	NAL_SEI,
	NAL_SPS,
	NAL_PPS,
	NAL_AUD,
	NAL_END_SEQUENCE,
	NAL_END_STREAM,
	NAL_FILLER_DATA,
	NAL_SPS_EXT,
	NAL_AUXILIARY_SLICE=19
};

unsigned int ParsePes(unsigned char *pInData, unsigned int uDataLen,
								unsigned char* pOut, unsigned long long *pOutPts)
{
    //PES长度判断
    if(uDataLen < 10)
    {
        fprintf(stderr, "Length of pes is incorrect, Pes len %u\n", uDataLen);
        return 0;
    }

    //PES包开始码判断
    if((pInData[0] != 0x00) || (pInData[1] != 0x00) || (pInData[2] != 0x01))
    {
        fprintf(stderr, "Head of pes is incorrect, 0x%02x%02x%02x\n", pInData[0], pInData[1], pInData[2]);
        return 0;
    }

    if (pInData[3] == STREAM_ID_PADDING) return 0;

	// Get PTS.
	GetPts(pInData, uDataLen, pOutPts);

	//unsigned int nOutLen = *(unsigned short*)(pInData + 4);
    unsigned int nOutLen = uDataLen - 6;
    unsigned int nOffset = 6;
    //STREAM_ID判断
    if(!((pInData[3] == STREAM_ID_MAP) ||
                (pInData[3] == STREAM_ID_1) ||
                (pInData[3] == STREAM_ID_2) ||
                (pInData[3] == STREAM_ID_ECM) ||
                (pInData[3] == STREAM_ID_EMM) ||
                (pInData[3] == STREAM_ID_DIR)))
    {
        unsigned int nPesHeaderLen = pInData[8];
        nOffset += 3;
        nOffset += nPesHeaderLen;
        nOutLen -= 3;
        nOutLen -= nPesHeaderLen;

		//printf("pOut is 0x%x pInData is 0x%x nOutLen is %d \n",pOut,pInData + nOffset, nOutLen);
        memcpy(pOut, pInData + nOffset, nOutLen);
        return nOutLen;

        /*if (!(pInData[nOffset] == 0x0 && pInData[nOffset+1] == 0x0 
                    && pInData[nOffset+2] == 0x0 && pInData[nOffset+3] == 0x1))
        {
            fprintf(stderr, "0001 assertion failed.\n");
            return 0;
        }

        unsigned int nPatchLen;
        unsigned char *pPatched = EsPatch(pInData + nOffset, nOutLen, &nPatchLen);
        printf("memcmp ... %s\n", memcmp(pPatched, pInData + nOffset, nOutLen) ? "failed" : "succeed");
        memcpy(pOut, pPatched, nPatchLen);
        free(pPatched);
        return nPatchLen;*/
    }

    return 0;
}

unsigned int ParsePes(unsigned char *pInData, unsigned int uDataLen, unsigned long long *pOutPts, unsigned int *pOffset)
{
    //PES长度判断
    if(uDataLen < 10)
    {
        fprintf(stderr, "Length of pes is incorrect, Pes len %u\n", uDataLen);
        return 0;
    }

    //PES包开始码判断
    if((pInData[0] != 0x00) || (pInData[1] != 0x00) || (pInData[2] != 0x01))
    {
        fprintf(stderr, "Head of pes is incorrect, 0x%02x%02x%02x\n", pInData[0], pInData[1], pInData[2]);
        return 0;
    }

    if (pInData[3] == STREAM_ID_PADDING) return 0;

	// Get PTS.
	if(pOutPts)
	{
		GetPts(pInData, uDataLen, pOutPts);
	}

	//unsigned int nOutLen = *(unsigned short*)(pInData + 4);
    unsigned int nOutLen = uDataLen - 6;
    unsigned int nOffset = 6;
    //STREAM_ID判断
    if(!((pInData[3] == STREAM_ID_MAP) ||
                (pInData[3] == STREAM_ID_1) ||
                (pInData[3] == STREAM_ID_2) ||
                (pInData[3] == STREAM_ID_ECM) ||
                (pInData[3] == STREAM_ID_EMM) ||
                (pInData[3] == STREAM_ID_DIR)))
    {
        unsigned int nPesHeaderLen = pInData[8];
        nOffset += 3;
        nOffset += nPesHeaderLen;
        nOutLen -= 3;
        nOutLen -= nPesHeaderLen;

		//printf("pOut is 0x%x pInData is 0x%x nOutLen is %d \n",pOut,pInData + nOffset, nOutLen);
        //memcpy(pOut, pInData + nOffset, nOutLen);
		
		if(pOffset)
		{
			*pOffset = nOffset;
		}

        return nOutLen;
    }

    return 0;
}

unsigned int ParsePesNotCpy(unsigned char *pInData, unsigned int nDataLen, unsigned char **pOut, unsigned long long *pOutPts, int *pScrambleFlag)
{
    //PES长度判断
    if (nDataLen < 10)
    {
        fprintf(stderr, "Length of pes is incorrect, Pes len %d\n", nDataLen);
        return 0;
    }

    //PES包开始码判断
    if ((pInData[0] != 0x00) || (pInData[1] != 0x00) || (pInData[2] != 0x01))
    {
        fprintf(stderr, "[ParsePesNotCpy] Head of pes is incorrect, 0x%02x%02x%02x\n", pInData[0], pInData[1], pInData[2]);
        return 0;
    }

    if (pInData[3] == STREAM_ID_PADDING) return 0;

    // Get PTS.
    GetPts(pInData, nDataLen, pOutPts);

    unsigned int nOutLen = nDataLen - 6;
    unsigned int nOffset = 6;
    //STREAM_ID判断
    if (!((pInData[3] == STREAM_ID_MAP) ||
        /*(pInData[3] == STREAM_ID_1) ||*/      // STREAM_ID_1 0xBD 是AC3音频 私有流 格式一样 正常解析即可
        (pInData[3] == STREAM_ID_2) ||
        (pInData[3] == STREAM_ID_ECM) ||
        (pInData[3] == STREAM_ID_EMM) ||
        (pInData[3] == STREAM_ID_DIR)))
    {
        // 判断加扰标志
        if(0x0 != ((pInData[6] >> 4) & 0x03))
        {
            if(pScrambleFlag)
                *pScrambleFlag = 1;

            return 0;
        }

        unsigned int nPesHeaderLen = pInData[8];
        nOffset += 3;
        nOffset += nPesHeaderLen;
        nOutLen -= 3;
        nOutLen -= nPesHeaderLen;

        *pOut = pInData + nOffset;   //返回es头起始地址

        return nOutLen;
        //*pOut = pInData + nOffset;
        //memcpy(pOut, pInData + nOffset, nOutLen);
        //return nOutLen;
    }

    return 0;
}

void GetPts(unsigned char *pInData, unsigned nDataLen, unsigned long long *ipPts)
{
	// Get PTS.
	if ( (pInData[6] & 0xC0 ) == 0x80 )
	{
		if (pInData[7] & 0x80 )
		{
			if (ipPts)
			{
				*ipPts = ((unsigned long long)(pInData[9] & 0x0e) << 29)
					| ((unsigned long long)(pInData[10] << 22))
					| ((unsigned long long)(pInData[11]&0xfe) << 14)
					| ((unsigned long long)(pInData[12] << 7))
					| ((unsigned long long)(pInData[13] >> 1));
			}
		}
	}
}

unsigned int GetPayload(unsigned char* pInData, unsigned char* pOutData, unsigned int nOutLen)
{
	unsigned int nPayloadLen = 0;

	unsigned char byAdapCtrl = GetAdapCtrl(pInData);
	unsigned int nPos = 4;

	//跳过调整字段
	if (byAdapCtrl & 0x02)
	{
		unsigned int nAdapLen = GetAdapLen(pInData);
		nPos += (nAdapLen + 1);
	}

	//判断有效负载长度是否正常
	if (nPos < TS_PACKET_LEN)
	{
		nPayloadLen = TS_PACKET_LEN - nPos;
	}

	if (nOutLen < nPayloadLen)
	{
        printf("[GetPayload] no enough buffer\n");
		return 0;
	}

	//拷贝数据(PES)
	memcpy(pOutData, pInData + nPos, nPayloadLen);

	return nPayloadLen;
}


bool FindIDRFrame(unsigned char *pEsData, unsigned int uLen)
{
	unsigned int uStartPos = 0, uEndPos = 0;
	unsigned char * pMove = pEsData;
	unsigned uLeftLen = uLen;

	while (uLeftLen > 0)
	{
		unsigned char byType = 0x00;
		if (false == FindNAL(pMove, uLeftLen, byType, uStartPos, uEndPos))
		{
			break;
		}

		if (NAL_IDR_SLICE == byType)
		{
			//printf("uLen: %d uLeftLen:%d Diff:%d\n", uLen, uLeftLen, uLen - uLeftLen);
			return true;
		}

		pMove += uEndPos;
		uLeftLen -= uEndPos;
	}

	return false;
}

bool FindNAL(unsigned char *pData, unsigned int nEsLen, unsigned char &byState,
					  unsigned int &nStartPos, unsigned int &nEndPos)
{
	UINT nPos = 0;
	unsigned char *pEnd = pData + nEsLen;
	BYTE byFlag = 0;

	while (pData + nPos + 6 < pEnd)
	{
		if ((pData[nPos + 0] == 0x00) && (pData[nPos + 1] == 0x00) && (pData[nPos + 2] == 0x01))
		{
			byFlag ++;

			if(1 == byFlag)
			{
				byState = pData[nPos + 3] & 0x1f;
				nStartPos = nPos + 5;
			}
			else if(2 == byFlag)
			{
				if ((nPos != 0) && (pData[nPos - 1] == 0x00))
				{
					nEndPos = nPos - 1;
				}
				else
				{
					nEndPos = nPos;
				}
				return true;
			}
		}
		nPos++;
	}

	if(1 == byFlag)
	{
		nEndPos = nEsLen;
		return true;
	}

	return false;
}

