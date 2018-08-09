#ifndef __OSD_MONITOR_H__
#define __OSD_MONITOR_H__

#include "common/Common.h"
#include "channel/ChannelStruct.h"

#ifdef __cplusplus  
extern "C"  
{  
#endif  
#include <libavfilter/avfiltergraph.h>  
#include <libavfilter/buffersink.h>  
#include <libavfilter/buffersrc.h>  
#include <libavutil/avutil.h>  
#include <libavutil/imgutils.h>  
#ifdef __cplusplus  
};  
#endif

//参数最大字符数量
#define INI_KEY_VALUE_MAXLEN			35
#define	OSD_TEXT_LATTICE_MAX_WIDTH		48		//目前点阵库中，字体最大宽度
#define OSD_TEXT_LATTICE_STRIDE			(INI_KEY_VALUE_MAXLEN*OSD_TEXT_LATTICE_MAX_WIDTH)		//点阵字符串最大宽度	，按像素算	

#define MAX_TEXT_SIZE_UTF8				(64*1024)
#define MAX_TEXT_SIZE_GB2312			(32*1024)

typedef	struct _OSD_TEXT_YUV_LATTICE_
{
	char yAddr[OSD_TEXT_LATTICE_STRIDE*OSD_TEXT_LATTICE_MAX_WIDTH];			//Y数据首地址
	//	char uvAddr[OSD_TEXT_LATTICE_STRIDE*OSD_TEXT_LATTICE_MAX_WIDTH/2];		//UV数据首地址
	int width;			//宽度
	int height;			//高度
	int stride;			//画幅
} OSD_TEXT_YUV_LATTICE;

class COsdMonitor
{
public:
	explicit COsdMonitor(void);
    ~COsdMonitor(void);

    void Init(int nW, int nH, int nFrameRate);
    void DeInit();

    void SetOsdInfo(OsdInfo & stOSD);

    void DrawOsdUseFfmpeg(mfxFrameSurface1 *pSurface);

	//叠加osd文字 pSurface， 需要叠加数据的 X、Y 坐标(位置)
	//int DrawOsd(unsigned char *pYAddr, int nWidth, int nHeight, int nPitch);

	//字符数据转换YUV数据
	//int OsdTextTransform(const char * word, int inLen, unsigned int fontSize, OSD_TEXT_YUV_LATTICE * yuvInfo);

	//捕获视频告警信息
	int CatchVideoMonitor(unsigned short VideoMonitor);

	//捕获音频告警信息
	int CatchAudioMonitor(unsigned short AudioMonitor);

private:
	//设置视频起始点位置
	//void SetTextPosition(int nInHeight, int nInPicth, int textWidth, int textHeight, int timerWidth, int timerHeight);

    void ReInitFilterDesc();
    void InitFilter(const char * filter_desc);

private:
    std::string			m_strOsdText;       //Text	文本内容
    std::string         m_strBase;
    std::string         m_strAlarmBase;
    std::string         m_strAlarmFilter;

    unsigned long long  m_ullOsdTm;
    bool                m_bReInit;

	int		            m_nFontSize;        //FontSize	字体大小  对48像素支持有问题
	int					m_nPosition;        //Position	位置信息  （0-左上角；1-右上角；2-左下角；3-右下角）
	int					m_nAntiColor;       //是否反色（0-不启用反色；1-启动反色）
	int					m_nAlign;           //Align	对齐方式（0-左对齐；1-居中；2-右对齐）
	int					m_nType;            //Type	显示类型（1-第一行为text，第二行为时间信息。2-第一行为text，无时间信息。3-text和时间信息在一行显示）
	int					m_nX; 
	int					m_nY;

    int                 m_nWidth;       //视频宽度
    int                 m_nHeight;      //视频高度
    int                 m_nFrameRate;   //视频帧率

	int					m_nTimeWidth;
	int					m_nTimeHeight;
	int					m_nTextWidth;
	int					m_nTextHeight;

    unsigned char     * m_pFrameBufferIn;
    unsigned char     * m_pFrameBufferOut;

    AVFrame           * m_pInFrame;
    AVFrame           * m_pOutFrame;

    AVFilter          * m_pBufferSrc;
    AVFilter          * m_pBufferSink;

    AVFilterContext   * m_pBufferSrcCtx;
    AVFilterContext   * m_pBufferSinkCtx;

    AVFilterGraph     * m_pFilterGraph;

    AVFilterInOut     * m_pOutputs;
    AVFilterInOut     * m_pInputs;
    
#if 0
	OSD_TEXT_YUV_LATTICE    m_yuvTextInfo;
	OSD_TEXT_YUV_LATTICE    m_yuvTimerInfo;

	OSD_TEXT_YUV_LATTICE    m_yuvVideoMonitorInfo;
	OSD_TEXT_YUV_LATTICE    m_yuvAudioMonitorInfo;
    std::string				m_strVideoAlarmDescLast;
    std::string				m_strAudioAlarmDescLast;
#endif

	std::string             m_strVideoAlarmDesc;
	std::string				m_strAudioAlarmDesc;
};

#endif
