#include "EncoderTS.h"
#include "Common.h"
#include <stdint.h>

static unsigned long long g_ullSysTime = 0;

namespace VA
{

//#define _LOG

#ifdef _LOG
static const char * TS_FILE = "test.ts";
#endif

#define AV_PTS_MASK				(0x1FFFFFFFF)

CEncoderTs::CEncoderTs()
{
	//视频PCR默认值
	m_ndEncodeInfo.nPcrPid = 48;

    //视频输出PID默认值
    m_ndEncodeInfo.nVideoPid = 48;
    //视频类型从原始流中读出
    m_ndEncodeInfo.nVideoStreamType = AV_CODEC_UNKNOWN;

    //音频输出PID默认值
    m_ndEncodeInfo.nAudioPid = 49;
    //音频类型从原始流中读出
    m_ndEncodeInfo.nAudioStreamType = AV_CODEC_UNKNOWN;

    m_ndEncodeInfo.nProgNo = 1;
    m_ndEncodeInfo.nPmtPid = 100;

	memset(m_bytePAT, 0xff, sizeof(m_bytePAT));
	memset(m_bytePMT, 0xff, sizeof(m_bytePMT));
	memset(m_bytePCR, 0xff, sizeof(m_bytePCR));
	InitCRC32Table();

    m_byteVideo = (unsigned char * ) malloc(TS_PACKET_LEN * TS_PACKET_MAX_COUNT);

	m_pFnOutPut = NULL;
	m_pContext = NULL;

	m_ullLastSendPSITime = m_ullLastSendPcrTime = _INIT;

	m_uPatCC = m_uPmtCC = m_uVideoCC = m_uAudioCC = m_uPcrCC = 0;

	m_pMediaTSFile = NULL;

	m_bPMTVersionChange = false;
	m_bytePMTVersion = 0x01;

	m_bPureAudio = false;

	m_bHavePSI = false;
	m_bHavePCR = false;
	m_nAudioDataLen = 0;

	//默认 Pcr Vedio PID相同
	m_bPcrVPidEqual = true;
}

CEncoderTs::~CEncoderTs()
{
    if (m_byteVideo)
    {
        SAFE_FREE(m_byteVideo);
    }

	if (m_pMediaTSFile)
	{
		fclose(m_pMediaTSFile);
	}
}

void CEncoderTs::Init(int nPmtPid, int nPcrPid, int nVPid, int nAPid)
{
    m_ndEncodeInfo.nPmtPid = nPmtPid;
    m_ndEncodeInfo.nPcrPid = nPcrPid;
    m_ndEncodeInfo.nVideoPid = nVPid;
    m_ndEncodeInfo.nAudioPid = nAPid;

	if(m_ndEncodeInfo.nPcrPid != m_ndEncodeInfo.nVideoPid)
	{
		m_bPcrVPidEqual = false;
	}
}

void CEncoderTs::RegOutPutCallBack(FuncOutPutTs pFnOutPut, void* pContext)
{
	m_pFnOutPut = pFnOutPut;
	m_pContext = pContext;
}

void CEncoderTs::EncodeEs(unsigned char* pBuff, int nLen,
                          unsigned long long ullDTS, unsigned long long ullPTS,
                          int nStreamType, int nPid,
                          bool bVideo)
{
	if (NULL == pBuff || nLen <= 0 || NULL == m_pContext) return;

    if (bVideo)
    {
		if (m_ndEncodeInfo.nVideoStreamType == AV_CODEC_UNKNOWN || nStreamType != m_ndEncodeInfo.nVideoStreamType)
        {
            switch (nStreamType)
            {
            case AV_CODEC_V_MPEG1:
            case AV_CODEC_V_MPEG2:
            case AV_CODEC_V_MPEG4:
            case AV_CODEC_V_H263:
            case AV_CODEC_V_H264:
            case AV_CODEC_V_HEVC:
            case AV_CODEC_V_AVS:
            case AV_CODEC_V_PRESENTATION_GRAPHICS:
            case AV_CODEC_V_INTERACTIVE_GRAPHICS:
            case AV_CODEC_V_SUBTITLE:
            case AV_CODEC_V_VC1:
                {
                    m_ndEncodeInfo.nVideoStreamType = nStreamType;
                    m_bPMTVersionChange = true;

                    break;
                }
            default:
                {
                    log_error("Unsupported video stream type %d", nStreamType);
                    return;
                }
            }
        }
    }
    else
    {
        if (m_ndEncodeInfo.nAudioStreamType == AV_CODEC_UNKNOWN || nStreamType != m_ndEncodeInfo.nAudioStreamType)
        {
            switch (nStreamType)
            {
            case AV_CODEC_A_MPEG1:
            case AV_CODEC_A_MPEG2:
            case AV_CODEC_A_AAC:
            case AV_CODEC_A_MPEG4:
            case AV_CODEC_A_AVS:
            case AV_CODEC_A_DRA:
            case AV_CODEC_A_LPCM:
            case AV_CODEC_A_AC3:
            case AV_CODEC_A_DTS:
            case AV_CODEC_A_AC3_TRUE_HD:
            case AV_CODEC_A_AC3_PLUS:
            case AV_CODEC_A_DTS_HD:
            case AV_CODEC_A_DTS_HD_MASTER:
            case AV_CODEC_A_SECONDARY_AC3_PLUS:
            case AV_CODEC_A_SECONDARY_DTS_HD:
                {
                    m_ndEncodeInfo.nAudioStreamType = nStreamType;
                    m_bPMTVersionChange = true;

                    break;
                }
            default:
                {
                    log_error("Unsupported audio stream type %d", nStreamType);
                    return;
                }
            }
        }
    }

    if (m_ndEncodeInfo.nVideoStreamType == AV_CODEC_UNKNOWN)
    {
        return;
    }

    OS_TimeVal tmCur;
    OS_Time::GetLocalTime(&tmCur);
    g_ullSysTime = (tmCur.tv_sec * 1000) + (tmCur.tv_usec / 1000);

	unsigned char* pTsData = m_byteVideo;
	pTsData += m_nAudioDataLen;

	int nTsCount = m_nAudioDataLen / TS_PACKET_LEN;

	//PAT PMT PCR 和数据一起发送 start
	if (0 == m_ullLastSendPcrTime)
	{
		AddPCR(pTsData, ullPTS, nTsCount);
		AddPSI(pTsData, ullPTS, nTsCount);
	}
	//end

	if (m_bPureAudio && bVideo)
	{
		//纯音频转发在这里加上 PCR PSI信息
		if((g_ullSysTime - m_ullLastSendPcrTime > PCR_INTERVAL_TM))	//打 PCR包
		{
			m_bHavePCR = true;
		}

		if((g_ullSysTime - m_ullLastSendPSITime > PAT_PMT_INTERVAL_TM) || m_bPMTVersionChange)	//打 PAT PMT PCR包
		{
			m_bHavePCR = true;
			m_bHavePSI = true;
		}

		if (m_bHavePCR)
		{
			AddPCR(pTsData, ullPTS, nTsCount);
		}

		if (m_bHavePSI)
		{
			AddPSI(pTsData, ullPTS, nTsCount);
		}

		//纯音频 发送PAT PMT PCR信息
		m_nAudioDataLen = nTsCount * TS_PACKET_LEN;
		return;
	}

	unsigned char pTsHead[TS_PACKET_LEN] = {0};
	unsigned char pPesHead[20] = {0};
	int nPesHeadLen = 0;	//PES HEAD
	int nPesLen = 0;		//PES

	pTsHead[0] = 0x47;

	if(bVideo)
	{
		pTsHead[1] = m_ndEncodeInfo.nVideoPid >> 8;
		pTsHead[2] = m_ndEncodeInfo.nVideoPid;
	}
	else
	{
		pTsHead[1] = m_ndEncodeInfo.nAudioPid >> 8;
		pTsHead[2] = m_ndEncodeInfo.nAudioPid;
	}

	//PesHead
	pPesHead[0] = 0x00;
	pPesHead[1] = 0x00;
	pPesHead[2] = 0x01;

	pPesHead[6] = 0x84;		//data_align_indicator = 1;
	pPesHead[7] = 0xC0;		//PTS_DTS_flag = 0x03;
	pPesHead[8] = 0x0A;		//pes_header_data_length = 10

	nPesHeadLen = pPesHead[8] + 9;//pes head total length

    if(bVideo)//视频PES包长度可变
    {
        pPesHead[3] = 0xe0;		//stream_id = 0xe0;

        pPesHead[4] = 0x00;		//pes_length = 0x00;
        pPesHead[5] = 0x00;		//pes_length = 0x00;
    }
    else//音频PES包恒定长度
    {
        pPesHead[3] = 0xc0;		        //stream_id = 0xc0;

        pPesHead[4] = (nLen + nPesHeadLen - 6) >> 8;		//pes_length = 0x00;
        pPesHead[5] = (nLen + nPesHeadLen - 6) & 0xff;		//pes_length = 0x00;
    }

	//PTS
	pPesHead[9] = 0x30 + ((char)(ullPTS >> 29) & 0x0e) + 0x01;
	pPesHead[10] = (char)(ullPTS >> 22) & 0xff;
	pPesHead[11] = ((char)(ullPTS >> 14) & 0xfe) + 0x01;
	pPesHead[12] = (char)(ullPTS >> 7) & 0xff;
	pPesHead[13] = ((char)(ullPTS << 1) & 0xfe) + 0x01;

	//DTS
	pPesHead[14] = 0x10 + ((char)(ullDTS >> 29) & 0x0e) + 0x01;
	pPesHead[15] = (char)(ullDTS >> 22) & 0xff;
	pPesHead[16] = ((char)(ullDTS >> 14) & 0xfe) + 0x01;
	pPesHead[17] = (char)(ullDTS >> 7) & 0xff;
	pPesHead[18] = ((char)(ullDTS << 1) & 0xfe) + 0x01;

	nPesLen = nLen + nPesHeadLen;//pes len
	int nLeftLen = nPesLen;

	while (nLeftLen > 0)
	{
		int nPos = 0;
		pTsHead[3] = 0x10;				//adapation_field_control = 0x01;

		if(bVideo)
		{
			pTsHead[3] += m_uVideoCC;		//continuity_counter

			if(m_bPcrVPidEqual)
			{
				m_uPcrCC = m_uVideoCC;		//PCR CC保持和视频CC一致
			}

			m_uVideoCC++;
			m_uVideoCC &= 0x0f;
		}
		else
		{
			pTsHead[3] += m_uAudioCC;		//continuity_counter
			m_uAudioCC++;
			m_uAudioCC &= 0xf;
		}
		
		if(nLeftLen == nLen	+ nPesHeadLen)		
		{
			pTsHead[1] |= 0x40;	//头包设置start_indicator
		}
		else
		{
			pTsHead[1] &= 0xBF;
		}

		if(nLeftLen < TS_PACKET_LEN - 4) //拷贝TS头，并加入调整字段
		{
			int nAdapat_field_len = TS_PACKET_LEN - 5 - nLeftLen;

			char pAdapatData[TS_PACKET_LEN] = {0};
			pTsHead[3] += 0x20;
			memcpy(pTsData, pTsHead, 4);
			nPos = 4;

			pAdapatData[0] = nAdapat_field_len;
			if(nAdapat_field_len)
			{
				pAdapatData[1] = 0x00;
				memset(pAdapatData + 2, 0xff, nAdapat_field_len - 1);
			}
			memcpy(pTsData + nPos, pAdapatData, nAdapat_field_len + 1);
			nPos += (nAdapat_field_len + 1);
		}
		else //拷贝TS包头
		{
			memcpy(pTsData, pTsHead, 4);
			nPos = 4;
		}

		//拷贝有效负载，区别是否拷贝PES头
		if(nLeftLen == nLen + nPesHeadLen)
		{
			memcpy(pTsData + nPos, pPesHead, nPesHeadLen);
			nPos += nPesHeadLen;
			nLeftLen -=  nPesHeadLen;

			memcpy(pTsData + nPos, pBuff, TS_PACKET_LEN - nPos);
			nLeftLen -= (TS_PACKET_LEN - nPos);
			pBuff += (TS_PACKET_LEN - nPos);
		}
		else
		{
			memcpy(pTsData + nPos, pBuff, TS_PACKET_LEN - nPos);
			nLeftLen -= (TS_PACKET_LEN - nPos);
			pBuff += (TS_PACKET_LEN - nPos);
		}

		pTsData += TS_PACKET_LEN;
		++nTsCount;

		if (nTsCount == TS_PACKET_MAX_COUNT)
		{
            log_error("ts pack is too large !");
            break;
        }
	}

    if (nTsCount == 0)
    {
        return;
    }

	//音频数据暂存 等待视频数据有数据再发送
	if (bVideo || m_bPureAudio) // 视频 and 纯音频
	{
		if(0 != m_ullLastSendPcrTime)
		{
			//输出7*188 根据空包数据填充 PCR PAT PMT
			//int nEmptyNum = 7 - (nTsCount % 7);
			//if(7 != nEmptyNum)
			//{
			//	if(nEmptyNum < 3)	//PCR
			//	{
			//		m_bHavePCR = true;
			//	}
			//	else				//PAT PMT PCR
			//	{
			//		m_bHavePCR = true;
			//		m_bHavePSI = true;
			//	}
			//}
			
			if((g_ullSysTime - m_ullLastSendPcrTime > PCR_INTERVAL_TM))	//打 PCR包
			{
				m_bHavePCR = true;
			}

			if((g_ullSysTime - m_ullLastSendPSITime > PAT_PMT_INTERVAL_TM) || m_bPMTVersionChange)	//打 PAT PMT PCR包
			{
				m_bHavePCR = true;
				m_bHavePSI = true;
			}

			if (m_bHavePCR)
			{
				AddPCR(pTsData, ullPTS, nTsCount);
			}

			if (m_bHavePSI)
			{
				AddPSI(pTsData, ullPTS, nTsCount);
			}
		}

		if (m_pFnOutPut)
        {
			m_pFnOutPut(m_pContext, m_byteVideo, TS_PACKET_LEN * nTsCount, bVideo);	//发送数据
        }
		m_nAudioDataLen = 0;

#ifdef _LOG
		if (!m_pMediaTSFile)
		{
			m_pMediaTSFile = fopen(TS_FILE, "w+");
		}

		if (m_pMediaTSFile)
		{
			fwrite(m_byteVideo, TS_PACKET_LEN * nTsCount, 1, m_pMediaTSFile);
			fflush(m_pMediaTSFile);
		}
#endif
	}
	else
	{
		if (nTsCount >= 7)
		{
			if (m_pFnOutPut)
			{
				m_pFnOutPut(m_pContext, m_byteVideo, TS_PACKET_LEN * nTsCount, bVideo);
			}
			m_nAudioDataLen = 0;

#ifdef _LOG
			if (!m_pMediaTSFile)
			{
				m_pMediaTSFile = fopen(TS_FILE, "w+");
			}

			if (m_pMediaTSFile)
			{
				fwrite(m_byteVideo, TS_PACKET_LEN * nTsCount, 1, m_pMediaTSFile);
				fflush(m_pMediaTSFile);
			}
#endif
		}
		else
		{
			m_nAudioDataLen = nTsCount * TS_PACKET_LEN; // 音频数据不够 7*188，凑齐再发
		}
	}
}

void CEncoderTs::InputTs(unsigned char* pData, int nLen, int nStreamType, int nPid, bool bIsAudio)
{
	if (!m_pContext) return;

    // 这里只有音频数据会走进来，音频数据直接转发
    // 如果信源只有音频，没有PCR信息，TODO

	if (!bIsAudio || m_ndEncodeInfo.nVideoStreamType <= 0) return;

    if (m_ndEncodeInfo.nAudioStreamType == AV_CODEC_UNKNOWN)
    {
#if 0
        if (nStreamType != AV_CODEC_A_AAC)
        {
            log_error("Unsupported audio stream type %d", nStreamType);
            return;
        }
#endif

        m_ndEncodeInfo.nAudioStreamType = nStreamType;
        m_bPMTVersionChange = true;

        GenPAT();
		memcpy(m_byteVideo + m_nAudioDataLen, m_bytePAT, TS_PACKET_LEN);
		m_nAudioDataLen += TS_PACKET_LEN;

        GenPMT();
		memcpy(m_byteVideo + m_nAudioDataLen, m_bytePMT, TS_PACKET_LEN);
		m_nAudioDataLen += TS_PACKET_LEN;
    }

	memcpy(m_byteVideo + m_nAudioDataLen, pData, nLen);
	m_nAudioDataLen += nLen;

	if (m_nAudioDataLen >= 7 * TS_PACKET_LEN)
	{
		if (m_pFnOutPut)
		{
			m_pFnOutPut(m_pContext, m_byteVideo, m_nAudioDataLen, false);
		}

#ifdef _LOG
		if (!m_pMediaTSFile)
		{
			m_pMediaTSFile = fopen(TS_FILE, "w+");
		}

		if (m_pMediaTSFile)
		{
			fwrite(m_byteVideo, m_nAudioDataLen, 1, m_pMediaTSFile);
			fflush(m_pMediaTSFile);
		}
#endif
        m_nAudioDataLen = 0;
	}

	return;
}

void CEncoderTs::AddPCR(unsigned char *&pTsData,  unsigned long long ullPTS, int &nTsCount)
{
	GenPCR((ullPTS - 4500) & AV_PTS_MASK);
	m_ullLastSendPcrTime = g_ullSysTime;

	//数据需要和 PCR 一起发送
	memcpy(pTsData, m_bytePCR, TS_PACKET_LEN);

	pTsData += TS_PACKET_LEN;
	nTsCount += 1;

	m_bHavePCR = false;
}

void CEncoderTs::AddPSI(unsigned char *&pTsData,  unsigned long long ullPTS, int &nTsCount)
{
	GenPAT();
	GenPMT();

	m_ullLastSendPSITime = g_ullSysTime;

	//数据需要和 PAT PMT 一起发送
	memcpy(pTsData, m_bytePAT, TS_PACKET_LEN);
	memcpy(pTsData + TS_PACKET_LEN, m_bytePMT, TS_PACKET_LEN);

	pTsData += TS_PACKET_LEN * 2;
	nTsCount += 2;

	m_bHavePSI = false;
}

void CEncoderTs::GenPAT()
{
	if(m_bytePAT[0] != 0x47)
	{
		m_bytePAT[0] = 0x47;
		m_bytePAT[1] = (0x01 << 6);		//payload_unit_start_indicator = 1
		m_bytePAT[2] = 0x00;
		m_bytePAT[3] = 0x10 + m_uPatCC;	//adapation_field = 0x01

		m_uPatCC ++; m_uPatCC &= 0x0f;

		m_bytePAT[4] = 0;				//pointer_field = 0
		/**********************************************/

		//section begin
		m_bytePAT[5] = 0x00;			//table_id = 0
		m_bytePAT[6] = 0xb0;			//section_syntax_indicator/section_length(h4)
		m_bytePAT[7] = 0x0d;			//section_length = 13
		m_bytePAT[8] = 0x00;			//transport_stream_id
		m_bytePAT[9] = 0x69;			//transport_stream_id
		m_bytePAT[10] = 0x03;			//version_number = 1; current_next_indicator = 1
		m_bytePAT[11] = 0x00;			//section_number = 0
		m_bytePAT[12] = 0x00;			//last_section_number = 0

		m_bytePAT[13] = 0x00;
		m_bytePAT[14] = m_ndEncodeInfo.nProgNo;			//program_number
		m_bytePAT[15] = m_ndEncodeInfo.nPmtPid >> 8;	//pmt_pid
		m_bytePAT[16] = m_ndEncodeInfo.nPmtPid;			//pmt_pid

		unsigned int uCRC32 = CheckCRC(&m_bytePAT[5], &m_bytePAT[17]);

		//CRC
		m_bytePAT[17] = uCRC32 >> 24;
		m_bytePAT[18] = uCRC32 >> 16;
		m_bytePAT[19] = uCRC32 >> 8;
		m_bytePAT[20] = uCRC32;
	}
	else
	{
		m_bytePAT[3] = 0x10 + m_uPatCC;	//adapation_field = 0x01

		m_uPatCC ++; m_uPatCC &= 0x0f;
	}
}

void CEncoderTs::GenPMT()
{
	if (m_bytePMT[0] != 0x47 || m_bPMTVersionChange)
	{
        m_bytePMT[0] = 0x47;
        m_bytePMT[1] = (0x01 << 6);		//payload_unit_start_indicator = 1
        m_bytePMT[1] += m_ndEncodeInfo.nPmtPid >> 8;
        m_bytePMT[2] = m_ndEncodeInfo.nPmtPid;
        m_bytePMT[3] = 0x10 + m_uPmtCC;	//adapation_field = 0x01

        m_uPmtCC ++; m_uPmtCC &= 0x0f;

        m_bytePMT[4] = 0;				//pointer_field = 0
        /**********************************************/

        //section begin
        m_bytePMT[5] = 0x02;							//table_id = 2
        m_bytePMT[6] = 0xb0;							//section_syntax_indicator/section_length(h4)
        m_bytePMT[7] = 0x17;							//section_length = 23
        m_bytePMT[8] = m_ndEncodeInfo.nProgNo >> 8;		//program_number
        m_bytePMT[9] = m_ndEncodeInfo.nProgNo;			//program_number

        m_bytePMT[10] |= (m_bytePMTVersion << 1);		//version_number++; 
        m_bytePMTVersion ++; m_bytePMTVersion &= 0x1f;

        m_bytePMT[10] |= 0x01;							//current_next_indicator = 1
        m_bytePMT[11] = 0x00;							//section_number = 0
        m_bytePMT[12] = 0x00;							//last_section_number = 0

        //m_bytePMT[13] = 0x1F;							//PCR Pid 0x1FFF
        //m_bytePMT[14] = 0xFF;							//PCR Pid 0x1FFF

		if(m_bPcrVPidEqual)
		{
			m_bytePMT[13] = m_ndEncodeInfo.nVideoPid >> 8;	//PCR Pid VideoPid
			m_bytePMT[14] = m_ndEncodeInfo.nVideoPid;		//PCR Pid VideoPid
		}
		else
		{
			m_bytePMT[13] = m_ndEncodeInfo.nPcrPid >> 8;	//PCR Pid 
			m_bytePMT[14] = m_ndEncodeInfo.nPcrPid;			//PCR Pid 
		}

        m_bytePMT[15] = 0x00;							//program_info_length = 0x00;
        m_bytePMT[16] = 0x00;							//program_info_length = 0x00;

        unsigned int uCRC32 = 0;

        if (m_bPureAudio && AV_CODEC_UNKNOWN != m_ndEncodeInfo.nAudioStreamType) //纯音频转发
        {
            m_bytePMT[7] = 0x12;							//section_length = 23

            m_bytePMT[17] = m_ndEncodeInfo.nAudioStreamType;//stream_type

            m_bytePMT[18] = m_ndEncodeInfo.nAudioPid >> 8;	//Audio Pid
            m_bytePMT[19] = m_ndEncodeInfo.nAudioPid;		//Audio Pid

            m_bytePMT[20] = 0x00;
            m_bytePMT[21] = 0x00;							//ES_info_length

            //CRC
            uCRC32 = CheckCRC(&m_bytePMT[5], &m_bytePMT[22]);
            m_bytePMT[22] = uCRC32 >> 24;
            m_bytePMT[23] = uCRC32 >> 16;
            m_bytePMT[24] = uCRC32 >> 8;
            m_bytePMT[25] = uCRC32;
        }
        else if (AV_CODEC_UNKNOWN == m_ndEncodeInfo.nAudioStreamType && AV_CODEC_UNKNOWN != m_ndEncodeInfo.nVideoStreamType)
        { // 信源只有视频，无音频
            m_bytePMT[7] = 0x12;							//section_length = 23

            m_bytePMT[17] = m_ndEncodeInfo.nVideoStreamType;//stream_type
            m_bytePMT[18] = m_ndEncodeInfo.nVideoPid >> 8;	//Video Pid
            m_bytePMT[19] = m_ndEncodeInfo.nVideoPid;		//Video Pid

            m_bytePMT[20] = 0x00;
            m_bytePMT[21] = 0x00;							//ES_info_length

            //CRC
            uCRC32 = CheckCRC(&m_bytePMT[5], &m_bytePMT[22]);
            m_bytePMT[22] = uCRC32 >> 24;
            m_bytePMT[23] = uCRC32 >> 16;
            m_bytePMT[24] = uCRC32 >> 8;
            m_bytePMT[25] = uCRC32;
        }
        else
        {
            m_bytePMT[17] = m_ndEncodeInfo.nVideoStreamType;//stream_type
            if (AV_CODEC_UNKNOWN == m_ndEncodeInfo.nVideoStreamType)
                log_error("video stream type err: not set\n");

            m_bytePMT[18] = m_ndEncodeInfo.nVideoPid >> 8;	//Video Pid
            m_bytePMT[19] = m_ndEncodeInfo.nVideoPid;		//Video Pid

            m_bytePMT[20] = 0x00;
            m_bytePMT[21] = 0x00;							//ES_info_length

            m_bytePMT[22] = m_ndEncodeInfo.nAudioStreamType;//stream_type
            if (AV_CODEC_UNKNOWN == m_ndEncodeInfo.nAudioStreamType)
                log_error("audio stream type err: not set\n");
			m_bytePMT[23] = m_ndEncodeInfo.nAudioPid >> 8;	//Audio Pid
			m_bytePMT[24] = m_ndEncodeInfo.nAudioPid;		//Audio Pid
            
			m_bytePMT[25] = 0x00;
            m_bytePMT[26] = 0x00;							//ES_info_length

            //CRC
            uCRC32 = CheckCRC(&m_bytePMT[5], &m_bytePMT[27]);
            m_bytePMT[27] = uCRC32 >> 24;
            m_bytePMT[28] = uCRC32 >> 16;
            m_bytePMT[29] = uCRC32 >> 8;
            m_bytePMT[30] = uCRC32;
        }

        m_bPMTVersionChange = false;
	}
 	else
 	{
 		m_bytePMT[3] = 0x10 + m_uPmtCC;	//adapation_field = 0x01
 
 		m_uPmtCC ++; m_uPmtCC &= 0x0f;
 	}
}

void CEncoderTs::GenPCR(unsigned long long ullPCR)
{
	memset(m_bytePCR, 0xff, TS_PACKET_LEN);

	m_bytePCR[0] = 0x47;

	if(m_bPcrVPidEqual)
	{
		m_bytePCR[1] = m_ndEncodeInfo.nVideoPid >> 8;
		m_bytePCR[2] = m_ndEncodeInfo.nVideoPid;
	}
	else
	{
		m_bytePCR[1] = m_ndEncodeInfo.nPcrPid >> 8;
		m_bytePCR[2] = m_ndEncodeInfo.nPcrPid;
	}

	m_bytePCR[3] = 0x20 + m_uPcrCC;	//adapation_field = 0x02

	if (m_bPureAudio || !m_bPcrVPidEqual)
	{
		m_uPcrCC ++;
		m_uPcrCC &= 0x0f;
	}

	m_bytePCR[4] = 183;			//adapation_lenght = 183
	//m_bytePCR[5] |= 0x80;		//discontinuity_indicator = 1
    m_bytePCR[5] = 0x10;		//pcr_flag = 1

	//PCR
	m_bytePCR[6] = (char)(ullPCR >> 25);
	m_bytePCR[7] = (char)(ullPCR >> 17);
	m_bytePCR[8] = (char)(ullPCR >> 9);
	m_bytePCR[9] = (char)(ullPCR >> 1);
	m_bytePCR[10] = ((char)(ullPCR & 0x01) << 7) + 0x7e;
	m_bytePCR[11] = 0x00;
}

void CEncoderTs::InitCRC32Table()
{
	unsigned int temp;
	for(int i = 0; i < 256; i++)
	{
		temp = i << 24;
		for(int j = 0; j < 8; j++)
		{
			if(temp & 0x80000000)
			{
				temp = (temp << 1)^POLY;
			}
			else
			{
				temp = temp << 1;
			}
		}
		m_nCrc32Table[i] = temp;
	}
}

unsigned int CEncoderTs::CheckCRC(unsigned char* pBegin, unsigned char* pEnd)
{
	unsigned int CRC = 0xffffffff;
	while(pBegin < pEnd)
	{
		CRC = (CRC << 8) ^ m_nCrc32Table[(CRC>>24)^((unsigned int)*pBegin)];
		pBegin ++;
	}

	return CRC;
}

bool CEncoderTs::Demux(unsigned char* buf, unsigned char*& data, bool& is_start, unsigned long long& i_dts, unsigned long long& i_pts)
{
	i_dts = i_pts = _INIT;

	is_start = false;
	unsigned char * head = (unsigned char*)buf;
	unsigned char * nextbuf = head + TS_PACKET_LEN;
	//	if( head[1] & 0x80 )   // 传输错误
	//  {
	//    return S_FALSE;
	// }

	if ( head[3] & 0x20 ) /* buf contains an adaptation field */
	{	
		data = head + 5 + head[4];
		if (data > nextbuf) 
			return false;
	}
	else
	{
		data = head + 4;
	}

	//Pes header
	if ((head[3] & 0x10) == 0) //payload
	{
		return false;
	}

	// 判断连续计数值是否正常，只包含调整
	//if (g_Config.CheckTSHeader())
	//{
	//	if (IsCCError(head, m_cc))
	//	{
	//		FMT_LOG("<%s, PID = %d>连续计数错", m_strURL, m_demux_pid);
	//		return S_FALSE;
	//	}
	//}

	// 不是PES开头，不做进一步的判断，直接返回成功
	if((head[1] & 0x40) == 0)
	{
		return true;
	}

	// 如果ES被加扰，返回失败
	if (data[0] != 0x00 || data[1] != 0x00 || data[2] != 0x01)
	{
		return false;
	}

	// 清流，继续解析
	is_start = true;

	unsigned int frame_size = (data[4] << 8) | data[5];

	if(frame_size > 0) 
		frame_size += 6;

	int i_skip = 0;
	/* TODO check size */
	switch( data[3] )
	{
	case 0xBC:  /* Program stream map */
	case 0xBE:  /* Padding */
	case 0xBF:  /* Private stream 2 */
	case 0xB0:  /* ECM */
	case 0xB1:  /* EMM */
	case 0xFF:  /* Program stream directory */
	case 0xF2:  /* DSMCC stream */
	case 0xF8:  /* ITU-T H.222.1 type E stream */
		i_skip = 6;
		break;
	default:
		if( ( data[6]&0xC0 ) == 0x80 )
		{
			/* mpeg2 PES */
			i_skip = data[8] + 9;

			if( data[7]&0x80 )    /* has pts(33 bit) */
			{
				i_pts = ((unsigned long long)(data[ 9]&0x0e ) << 29)|
					(unsigned long long)(data[10] << 22)|
					((unsigned long long)(data[11]&0xfe) << 14)|
					(unsigned long long)(data[12] << 7)|
					(unsigned long long)(data[13] >> 1);

				if( data[7]&0x40 )    /* has dts(33 bit) */
				{
					i_dts = ((unsigned long long)(data[14]&0x0e ) << 29)|
						(unsigned long long)(data[15] << 22)|
						((unsigned long long)(data[16]&0xfe) << 14)|
						(unsigned long long)(data[17] << 7)|
						(unsigned long long)(data[18] >> 1);
				}
				else 
				{
					i_dts = i_pts;
				}
			}
		}
	}
	data += i_skip;
	if (data >= nextbuf)
	{
		return false;
	}

	return true;
}
}
