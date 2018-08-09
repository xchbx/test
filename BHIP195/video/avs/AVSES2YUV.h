#pragma once

#include "xavs.h"
#include "xavs_globe.h"
#include "xavs_dsp.h"
#include "xavs_decoder.h"
#include "VideoES2YUV.h"


class AVSES2YUV: public VideoES2YUV
{
public:
    AVSES2YUV();
    ~AVSES2YUV();

    bool Decode(FrameBitStream & frame);

private:
    bool Init();
    void DeInit();
    void WriteYUVDataI420();


private:
    AVSFrame        m_stAVSFrame;
    void           *m_pAvsDecoder;
    
};

