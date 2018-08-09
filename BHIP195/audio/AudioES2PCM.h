#pragma once

typedef void (*CBF_WritePCMData)(unsigned char *, unsigned int, unsigned long long, unsigned int uSamplesNumber, void *);

class AudioES2PCM
{
public:
    AudioES2PCM(): m_pfnWritePCMData(NULL), m_lpContext(NULL) {}
    virtual ~AudioES2PCM() {}

    virtual bool Init(int nAudioType = 0) = 0;
    virtual void DeInit() = 0;

    //将ES数据 解析为PCM数据 并重新采样 放入环形 缓冲区中
    //这里要求 AudioReader模块 传入一个正的 ES数据包，防止不完整的ES数据包最后的数据丢失
    virtual void Decode(unsigned char* pES, int nLen, unsigned long long ullPts, unsigned long long ullDts) = 0;

    void RegisterCallbackFunc(CBF_WritePCMData pfnWritePCMData, void *lp)
    {
        m_pfnWritePCMData = pfnWritePCMData;
        m_lpContext = lp;
    }


public:
    CBF_WritePCMData		m_pfnWritePCMData;
    void				   *m_lpContext;
};

