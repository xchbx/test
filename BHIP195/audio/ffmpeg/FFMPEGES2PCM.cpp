#include "FFMPEGES2PCM.h"

FFMPEGES2PCM::FFMPEGES2PCM()
: m_pFormatCtx(NULL)
, m_pCodecCtx(NULL)
, m_pCodec(NULL)
, m_pAvParser(NULL)
, m_pPacket(NULL)
, m_pFrame(NULL)
, m_AvConvertCtx(NULL)
, m_nReSamplingOutRate(48000)
, m_ulReSamplingOutChannel(0)
, m_nOutChannel(2)
, m_bSwrInited(false)
, m_bFfmpegInit(false)
, m_nInChannel(2)
, m_nReSamplingInRate(48000)
, m_pSWRPcmData(NULL)
{
    m_pSWRPcmData = (unsigned char *) malloc(AUDIO_BUF_SIZE);
}

FFMPEGES2PCM::~FFMPEGES2PCM()
{
    DeInit();
}

bool FFMPEGES2PCM::Init(int nAudioType)
{
    // 根据AudioType进行处理   
    if(AV_CODEC_A_AAC == nAudioType)
    {
        m_enAudioType = AV_CODEC_ID_AAC;  
    }
    else if(AV_CODEC_A_AC3 == nAudioType)
    {
        m_enAudioType = AV_CODEC_ID_AC3;
    }
    else if(AV_CODEC_A_MPEG2 == nAudioType)
    {
        m_enAudioType = AV_CODEC_ID_MP3;
    }
    else
    {
        log_error("AudioType:%d Unsupport, Init Failed", nAudioType);
        return false;
    }

    // 重采样 目标参数
    m_enReSamplingOutFormat = AV_SAMPLE_FMT_S16;
    m_nReSamplingOutRate = AUDIO_SWR_SAMPLEING;
    m_ulReSamplingOutChannel = AV_CH_LAYOUT_STEREO;
    m_nOutChannel = AUDIO_SWR_CHANNELS;

    if(!FfmpegInit())
        return false;

    log_info("AudioType:%d Init Success", nAudioType);
    return true;
}

void FFMPEGES2PCM::DeInit()
{
    if (m_pSWRPcmData)
    {
        free(m_pSWRPcmData);
        m_pSWRPcmData = NULL;
    }

    FfmpegDeInit();
}

bool FFMPEGES2PCM::FfmpegInit()
{
    //初始化
    av_register_all();
    avformat_network_init();

    //根据 音频类型初始化 音频解码器,音频解码器上下文
    m_pCodec = avcodec_find_decoder(m_enAudioType);
    m_pCodecCtx = avcodec_alloc_context3(m_pCodec);
    if(!m_pCodec || !m_pCodecCtx)
    {
        printf("DecodeResample FfmpegInit Failed\n");
        return false;
    }

    //打开 音频解码器
    avcodec_open2(m_pCodecCtx, m_pCodec, NULL);
    //根据音频类型 初始化 ES音频解析器
    m_pAvParser = av_parser_init(m_enAudioType);

    //压缩 未压缩音频数据 内存申请
    m_pPacket =(AVPacket *)malloc(sizeof(AVPacket));
    av_init_packet(m_pPacket);
    m_pFrame = av_frame_alloc();

    m_bSwrInited = false;

    return true;
}

bool FFMPEGES2PCM::FfmpegDeInit(bool bFreeSWR)
{
    if (m_AvConvertCtx && bFreeSWR)
    {
        swr_close(m_AvConvertCtx);
        swr_free(&m_AvConvertCtx);
        m_AvConvertCtx = NULL;
    }

    if(m_pFrame)
        av_frame_free(&m_pFrame);

    if(m_pPacket)
        free(m_pPacket);

    if(m_pAvParser)
        av_parser_close(m_pAvParser);

    // Close the codec
    if(m_pCodecCtx)
    {
        avcodec_free_context(&m_pCodecCtx);
        avcodec_close(m_pCodecCtx);
    }

    // Close the video file
    if(m_pFormatCtx)
        avformat_close_input(&m_pFormatCtx);

    m_pFormatCtx = NULL;
    m_pCodecCtx = NULL;
    m_pCodec = NULL;
    m_pAvParser = NULL;
    m_pPacket = NULL;
    m_pFrame = NULL;

    return true;
}

void FFMPEGES2PCM::Decode(unsigned char* pES, int nLen, unsigned long long ullPts, unsigned long long ullDts)
{
    if (NULL == pES || nLen <= 0)
        return ;

    unsigned char *pInData = pES;
    int nInLen = nLen;

    while (nInLen)
    {
        unsigned char *pOutBuff = NULL;
        int nOutSize = 0;

        //使用 ES解析器 将ES数据 解析为压缩的PCM数据到 pOutBuff指向的内存中
        int nAvParser2Ret = av_parser_parse2(m_pAvParser, m_pCodecCtx, &pOutBuff, &nOutSize, pInData, nInLen, ullPts, ullDts, 0);

        pInData += nAvParser2Ret;
        nInLen -= nAvParser2Ret;

        if (nOutSize <= 0)
        {
            //只是这次的数据量不够，等待下次数据的到来 可能将数据先放到 m_pCodecCtx中去了
            //测试中发现 avcodec_decode_audio4要想解析 这个数据帧，必要要 250-400字节，如果传入的pES不够的话，会在这里返回，等待下次一起解析
            continue;
        }

        if (nOutSize > 0 && NULL != pOutBuff)
        {
            //将 上面解析的PCM数据 存放到 m_pPacket中
            m_pPacket->data = pOutBuff;
            m_pPacket->size = nOutSize;
            //int pts = m_pPacket->pts;

            int nSendRet = -1;
            int nReceiveRet = -1;

            //数据存放在 m_pPacket中 Send给音频解码器
            nSendRet = avcodec_send_packet(m_pCodecCtx, m_pPacket);

            if (nSendRet == 0)
            {
                //将解码器中解码后的数据 取出来放入 m_pFrame中
                nReceiveRet = avcodec_receive_frame(m_pCodecCtx, m_pFrame);

                if(0 != nReceiveRet)
                {
                    char cAvReceiveErrorInfo[1024] = {0};
                    av_strerror(nReceiveRet, cAvReceiveErrorInfo, 1023);
                    log_warning("Receive: %s", cAvReceiveErrorInfo);
                }
            }
            else
            {
                char cAvSendErrorInfo[1024] = {0};
                av_strerror(nSendRet, cAvSendErrorInfo, 1023);
                log_warning("Send: %s", cAvSendErrorInfo);

                if (nSendRet == -11 || nSendRet == 11)
                {
                    FfmpegDeInit(false);
                    FfmpegInit();
                    log_error("Ffmpeg Resource not Avaiable , Ffmpeg ReInit");
                }

                return;
            }

            //avcodec_decode_audio4 函数调用之后 m_pCodecCtx可以获取到 输入音频的各种参数
            if (!m_bSwrInited && 0 == nReceiveRet)
            {
                //重采样器 初始化
                m_AvConvertCtx = swr_alloc();

                //设置 音频 输入输出参数：声道数 格式 采样率
                m_AvConvertCtx = swr_alloc_set_opts(m_AvConvertCtx, 
                    m_ulReSamplingOutChannel, m_enReSamplingOutFormat, m_nReSamplingOutRate, 
                    m_pCodecCtx->channel_layout, m_pCodecCtx->sample_fmt, m_pCodecCtx->sample_rate, 0, NULL);

                m_nInChannel = m_pCodecCtx->channels;
                m_nReSamplingInRate = m_pCodecCtx->sample_rate;

                log_info("SWR: Out-> ChannelLayout:%lu ChannelNumber:%d Format:%d Rate:%d", m_ulReSamplingOutChannel, m_nOutChannel, m_enReSamplingOutFormat, m_nReSamplingOutRate);
                log_info("SWR: In -> ChannelLayout:%lu ChannelNumber:%d Format:%d Rate:%d", m_pCodecCtx->channel_layout, m_pCodecCtx->channels, m_pCodecCtx->sample_fmt, m_pCodecCtx->sample_rate);

                int nSwrInitRet = swr_init(m_AvConvertCtx);
                //重采样器 初始化完成

                printf("Swr Success SwrInitRet:%d\n", nSwrInitRet);
                m_bSwrInited = true;
            }

            //nReceiveRet == 0 获取到了可以播放的音频PCM数据
            if (nReceiveRet == 0)
            {
                //判断是否需要重采样 

                int line_size = 0;
                //重采样后的 数据存放在 m_pPcmData中
                int ret = swr_convert(m_AvConvertCtx, &(m_pSWRPcmData), AUDIO_BUF_SIZE,
                    (const unsigned char **)m_pFrame->data, m_pFrame->nb_samples);

                //根据 声道数、采样格式 获取到向文件写入的数据大小
                int nSizeRet = av_samples_get_buffer_size(&line_size, m_nOutChannel, ret, m_enReSamplingOutFormat, 1);
                if(nSizeRet != line_size)
                {
                    fprintf(stderr, "av_samples_get_buffer_size ERROR, nSizeRet:%d line_size:%d\n", nSizeRet, line_size);
                }

                //line_size是缓冲区中数据的大小 可以写入文件  ret是这个缓冲区 对于Alsa来说 需要写入的大小
                if (nSizeRet > 0 && m_pfnWritePCMData)
                {
#if 1
					m_pfnWritePCMData(m_pSWRPcmData, nSizeRet, ullPts, m_pFrame->nb_samples, m_lpContext);
#else
					static FILE * fp = fopen("out.pcm", "wb+");
                    if (fp) fwrite(m_pSWRPcmData, 1, nSizeRet, fp);
#endif            
                }
            }


        }//end of av_parser_parse2 success
    }//end of while
}

