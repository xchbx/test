#include "AudioPCM2ES.h"


AudioPCM2ES::AudioPCM2ES()
: m_pCodec(NULL)
, m_pCodecCtx(NULL)
, m_pFrame(NULL)
, m_pFrameBuf(NULL)
, m_pBufAudioData(NULL)
, m_nBufAudioSize(0)
{
    m_pBufAudioData = (unsigned char *) malloc(40960);
}

AudioPCM2ES::~AudioPCM2ES()
{
    DeInit();
}

bool AudioPCM2ES::Init(int nAudioType, int nBitRate)
{
    if(nBitRate <= 0)
    {
        nBitRate = 64;
    }
    nBitRate *= 1000;   // bitRate转换为 bps

    avcodec_register_all();
    AVCodecID codec_id = AV_CODEC_ID_AAC;
    m_pCodec = avcodec_find_encoder_by_name("libfdk_aac");  // 加载libfdk_aac进行aac编码 否则新版本ffmpeg报错 无法编码
    //m_pCodec = avcodec_find_encoder(codec_id);

    if (!m_pCodec) 
    {
        log_error("Codec not found");
        return false;
    }

    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    if (!m_pCodecCtx) 
    {
        log_error("Could not allocate video codec context");
        return false;
    }

    m_pCodecCtx->codec_id = codec_id;
    m_pCodecCtx->codec_type = AVMEDIA_TYPE_AUDIO;
    m_pCodecCtx->sample_fmt = AV_SAMPLE_FMT_S16;
    m_pCodecCtx->sample_rate = AUDIO_SWR_SAMPLEING;
    m_pCodecCtx->channel_layout = AV_CH_LAYOUT_STEREO;
    m_pCodecCtx->channels = av_get_channel_layout_nb_channels(m_pCodecCtx->channel_layout);
    m_pCodecCtx->bit_rate = nBitRate;

    if (avcodec_open2(m_pCodecCtx, m_pCodec, NULL) < 0) 
    {
        log_error("Could not open codec");
        return -1;
    }

    m_pFrame = av_frame_alloc();
    m_pFrame->nb_samples = m_pCodecCtx->frame_size;
    m_pFrame->format = m_pCodecCtx->sample_fmt;
    int nSize = av_samples_get_buffer_size(NULL, m_pCodecCtx->channels, m_pCodecCtx->frame_size, m_pCodecCtx->sample_fmt, 1);   // size = 4096
    m_pFrameBuf = (unsigned char *)av_malloc(nSize);

    avcodec_fill_audio_frame(m_pFrame, m_pCodecCtx->channels, m_pCodecCtx->sample_fmt, (const uint8_t*)m_pFrameBuf, nSize, 1);
    av_init_packet(&m_stPacket);

    log_info("Init BitRate:%d bps  PerFrameSize:%d", nBitRate, nSize);
    return true;
}

void AudioPCM2ES::DeInit()
{   
    avcodec_close(m_pCodecCtx);
    av_free(m_pCodecCtx);
    av_freep(&m_pFrame->data[0]);
    av_frame_free(&m_pFrame);

    av_free_packet(&m_stPacket);
    SAFE_FREE(m_pBufAudioData);
}

void AudioPCM2ES::InputEncodeData(unsigned char *pInData, unsigned int uInLen, unsigned long long ullPTS)
{
    memcpy(m_pBufAudioData + m_nBufAudioSize, pInData, uInLen);
    m_nBufAudioSize += uInLen;
}

bool AudioPCM2ES::Encode(unsigned char* &pOutData, unsigned int &uOutLen)
{
    if (m_nBufAudioSize < AUDIO_AAC_PER_FRAME_SIZE)
    {
        return false;
    }

    m_stPacket.data = NULL;    // packet data will be allocated by the encoder
    m_stPacket.size = 0;

    // 每次拷贝AAC需要的帧数据量(4096)到m_pFrameBuf
    memcpy(m_pFrameBuf, m_pBufAudioData, AUDIO_AAC_PER_FRAME_SIZE);
    //m_pFrame->pts = ullPTS;

    m_nBufAudioSize -= AUDIO_AAC_PER_FRAME_SIZE;
    if(m_nBufAudioSize > 0)
    {
        // 将剩余的数据拷贝到缓冲区头部
        memmove(m_pBufAudioData, m_pBufAudioData + AUDIO_AAC_PER_FRAME_SIZE, m_nBufAudioSize);
    } 

    int nGetOutput = 0;
    int nRet = avcodec_encode_audio2(m_pCodecCtx, &m_stPacket, m_pFrame, &nGetOutput);
    if (nRet < 0) 
    {
        printf("Error encoding frame\n");
        return false;
    }

    if (nGetOutput) 
    {
        pOutData = m_stPacket.data;
        uOutLen = m_stPacket.size;

#if 0
        static FILE * fp = fopen("test.es", "wb+");
        if (fp)
        {
            fwrite(m_stPacket.data, m_stPacket.size, 1, fp);
        }
#endif

        return true;
    }

    return false;
}

