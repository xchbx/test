#include "DraES2PCM.h"


#define PCM_DATALEN     2048

DraES2PCM::DraES2PCM()
: m_bGetHeader(false)
, m_pDraDecoder(NULL)
, m_pPCMData(NULL)
, m_nInChannel(2)
, m_nReSamplingInRate(48000)
{

}

DraES2PCM::~DraES2PCM()
{
    DeInit();
}

bool DraES2PCM::Init(int nAudioType)
{
    // 根据AudioType进行处理   
    m_pDraDecoder = xdra_decoder_create(NULL, 0);
    if(NULL == m_pDraDecoder)
        return false;

    m_pPCMData = (short *)malloc(PCM_DATALEN * sizeof(short));
    if(NULL == m_pPCMData)
        return false;

    printf("DraES2PCM Init Success\n");
    return true;
}

void DraES2PCM::DeInit()
{
    if(m_pDraDecoder)
    {
        xdra_decoder_destroy(m_pDraDecoder);
    }

    SAFE_FREE(m_pPCMData);
    m_bGetHeader = false;
}

void DraES2PCM::Decode(unsigned char* pES, int nLen, unsigned long long ullPts, unsigned long long ullDts)
{
    if(0 >= nLen)
        return ;

    if(0 != xdra_decoder_put_data(m_pDraDecoder, pES, nLen))
        return ;

    xdra_frame_header stHeader;
    while(1)
    {
        if(xdra_decoder_decode_next_frame(m_pDraDecoder) != 0)
            break;

        xdra_decoder_get_frame_header(m_pDraDecoder, &stHeader);
        if(!m_bGetHeader)
        {
            m_nReSamplingInRate = xdra_get_sample_rate(stHeader.sample_rate_index);
            m_nInChannel = stHeader.num_normal_channel + stHeader.num_lfe_channel;
            int nBitRate = m_nReSamplingInRate * stHeader.num_word / 32;
            nBitRate = (nBitRate + 16000)/32000 * 32000;

            printf("sample rate = %d,  normal channel number = %d, lfe channel number = %d bitrate about %dbps\n", 
                                    m_nReSamplingInRate, stHeader.num_normal_channel, stHeader.num_lfe_channel, nBitRate);		
            if(m_nInChannel > 2)
            {
                m_nInChannel = 2;
            }

            m_bGetHeader = true;
        }

        int ret_len = 0;
        if(stHeader.num_normal_channel + stHeader.num_lfe_channel < 2)
        {
            ret_len = xdra_decoder_get_channel_s16(m_pDraDecoder, 0, m_pPCMData, PCM_DATALEN);
        }
        else
        {
            ret_len = xdra_decoder_get_stero_s16(m_pDraDecoder, m_pPCMData, PCM_DATALEN);
        }

        if(ret_len > 0 && m_pfnWritePCMData)
        {
#if 1
            m_pfnWritePCMData((unsigned char *)m_pPCMData, ret_len * 2, ullPts, 1024, m_lpContext);
#else
            printf("DraES2PCM PCMSize:%d\n", ret_len * 2);  
            static FILE *m_fDest = fopen("./dra.pcm", "wb");
            mfxU32 nBytesWritten = (mfxU32)fwrite(m_pPCMData, 1, ret_len * 2, m_fDest);
#endif
        }

    }
}

