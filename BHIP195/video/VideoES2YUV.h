#pragma once

#include "ChannelStruct.h"

typedef void (*CBF_WriteYUVData)(void *, unsigned long long, int nFrameRate, void *);


class VideoES2YUV
{
public:
    VideoES2YUV(): m_pfnWriteYUVData(NULL), m_lpContext(NULL) {}
    virtual ~VideoES2YUV() {}

    virtual bool Decode(FrameBitStream & frame) = 0;

    void RegisterCallbackFunc(CBF_WriteYUVData pfnWriteYUVData, void *lp)
    {
        m_pfnWriteYUVData = pfnWriteYUVData;
        m_lpContext = lp;
    }

public:
    CBF_WriteYUVData        m_pfnWriteYUVData;
    void				  * m_lpContext;
};

