#pragma once 


#include "AudioCommon.h"
#include "AudioES2PCM.h"

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
class FFMPEGES2PCM : public AudioES2PCM
{
public:
    FFMPEGES2PCM();
    ~FFMPEGES2PCM();

    bool Init(int nAudioType);
    void DeInit();


    //将ES数据 解析为PCM数据 并重新采样 放入环形 缓冲区中
    //这里要求 AudioReader模块 传入一个正的 ES数据包，防止不完整的ES数据包最后的数据丢失
    void Decode(unsigned char* pES, int nLen, unsigned long long ullPts, unsigned long long ullDts);


private:
    bool		FfmpegInit();
    bool		FfmpegDeInit(bool bFreeSWR = true);


private:
    //ffmpeg 读取数据 重采样私有变量
    enum AVCodecID			m_enAudioType;		//音频类型 AAC(1024) MP3(1152)
    AVFormatContext	      * m_pFormatCtx;		//ffmpeg全局上下文
    AVCodecContext		  * m_pCodecCtx;		//解码器上下文（输入音频的 属性信息 全都在这个变量中保存：采样率、采样格式、声道）
    AVCodec				  * m_pCodec;			//解码器指针
    AVCodecParserContext  * m_pAvParser;		//ES数据解析器
    AVPacket			  * m_pPacket;			//音频 压缩数据
    AVFrame				  * m_pFrame;			//音频 未压缩数据

    //重采样 参数
    struct SwrContext	  * m_AvConvertCtx;	//重采样 上下文

    //重采样 目标参数(通过配置文件传入)
    enum AVSampleFormat		m_enReSamplingOutFormat;	//采样格式
    int						m_nReSamplingOutRate;		//采样率
    unsigned long			m_ulReSamplingOutChannel;	//声道数 单声道 4 立体声 3
    int						m_nOutChannel;				//声道数 单声道 1 立体声 2

    bool					m_bSwrInited;				//重采样器是否已经初始化了
    

private:
    bool					m_bFfmpegInit;		    //ffmpeg    
    int						m_nInChannel;			//信源 声道数
    int						m_nReSamplingInRate;	//信源 采样率

    unsigned char		  * m_pSWRPcmData;         //ES数据重采样之后的数据,存放到下面的环形缓冲区中
};

