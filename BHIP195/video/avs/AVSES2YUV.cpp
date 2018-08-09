#include "AVSES2YUV.h"


AVSES2YUV::AVSES2YUV()
: m_pAvsDecoder(NULL)
{
}

AVSES2YUV::~AVSES2YUV()
{
    DeInit();
}

bool AVSES2YUV::Init()
{
    xavs_decoder_create(&m_pAvsDecoder);
    if(NULL == m_pAvsDecoder)
        return false;

    return true;
}

void AVSES2YUV::DeInit()
{
    xavs_decoder_destroy(m_pAvsDecoder);
    m_pAvsDecoder = NULL;
}

bool AVSES2YUV::Decode(FrameBitStream & frame)
{
    if(NULL == m_pAvsDecoder)
    {
        if(!Init())
            return false;
    }

    if(frame.uLen == 0)
        return false;

    xavs_decoder_put_data(m_pAvsDecoder, frame.pData, frame.uLen);

    int nGotFrame = 0;
    while(1)
    {
        xavs_decoder_get_decode_video(m_pAvsDecoder, &nGotFrame, &m_stAVSFrame);
        if(nGotFrame == 0)
        {
            break;
        }

        //printf("index = %d type = %s coded = %s\n", 
        //    m_stAVSFrame.nFrameIndex, m_stAVSFrame.nFrameType == 0? "I Frame" : (m_stAVSFrame.nFrameType == 1? "P Frame":"B Frame"),
        //    m_stAVSFrame.nFrameCoded ? "frame" : "field");

#if 1
        m_pfnWriteYUVData((void *)(&m_stAVSFrame), frame.ullPts, 30, m_lpContext);  // FrameRate ToDo
#else
        WriteYUVDataI420();
#endif

        nGotFrame = 0;
    }

    return true;
}

void AVSES2YUV::WriteYUVDataI420()
{
    static FILE *fDest = fopen("./test.yuv", "wb");

    if(fDest)
    {
        int i;
        int first = 0;
        int second = 1;
        for(i=0;i<m_stAVSFrame.nHeight/2;i++)
        {
            fwrite(m_stAVSFrame.data[0]+m_stAVSFrame.linesize[0]*(i * 2 + first),1,m_stAVSFrame.nWidth,fDest);
            fwrite(m_stAVSFrame.data[0]+m_stAVSFrame.linesize[0]*(i * 2 + second),1,m_stAVSFrame.nWidth,fDest);
        }
        for(i=0;i<m_stAVSFrame.nHeight/4;i++)
        {
            fwrite(m_stAVSFrame.data[1]+m_stAVSFrame.linesize[1]*(i * 2 + first),1,m_stAVSFrame.nWidth/2,fDest);
            fwrite(m_stAVSFrame.data[1]+m_stAVSFrame.linesize[1]*(i * 2 + second),1,m_stAVSFrame.nWidth/2,fDest);
        }
        for(i=0;i<m_stAVSFrame.nHeight/4;i++)
        {
            fwrite(m_stAVSFrame.data[2]+m_stAVSFrame.linesize[2]*(i * 2 + first),1,m_stAVSFrame.nWidth/2,fDest);
            fwrite(m_stAVSFrame.data[2]+m_stAVSFrame.linesize[2]*(i * 2 + second),1,m_stAVSFrame.nWidth/2,fDest);
        }
    }
}

