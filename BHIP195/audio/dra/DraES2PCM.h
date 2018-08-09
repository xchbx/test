#pragma once

#include "AudioCommon.h"
#include "AudioES2PCM.h"
#include "xdra_decoder.h"


// Dra音频固定16bit(bitsPerSample)
class DraES2PCM: public AudioES2PCM
{
public:
    DraES2PCM();
    ~DraES2PCM();

    bool Init(int nAudioType);
    void DeInit();

    //将ES数据 解析为PCM数据 并重新采样 放入环形 缓冲区中
    //这里要求 AudioReader模块 传入一个正的 ES数据包，防止不完整的ES数据包最后的数据丢失
    void Decode(unsigned char* pES, int nLen, unsigned long long ullPts, unsigned long long ullDts);

private:
    bool                    m_bGetHeader;
    void                   *m_pDraDecoder;

    short                  *m_pPCMData;

    int						m_nInChannel;			//信源 声道数
    int						m_nReSamplingInRate;	//信源 采样率
};

