#pragma once 

#include "AudioCommon.h"
#ifdef __cplusplus  
extern "C"  
{  
#endif  
#include <libavutil/samplefmt.h>
#include <libavutil/opt.h>
#include <libavcodec/avcodec.h>
#include <libavutil/imgutils.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswresample/swresample.h>
#ifdef __cplusplus  
};  
#endif  


// 使用ffmpeg对音频进行格式转换
class AudioPCM2ES
{
public:
    AudioPCM2ES();
    ~AudioPCM2ES();

    bool Init(int nAudioType, int nBitRate);
    void DeInit();

    void InputEncodeData(unsigned char *pInData, unsigned int uInLen, unsigned long long ullPTS);
    bool Encode(unsigned char* &pOutData, unsigned int &uOutLen);


private:
    AVCodec         *m_pCodec;
    AVCodecContext  *m_pCodecCtx;
    AVFrame         *m_pFrame;

    unsigned char   *m_pFrameBuf;
    AVPacket        m_stPacket;


    unsigned char   *m_pBufAudioData;
    int             m_nBufAudioSize;
};

