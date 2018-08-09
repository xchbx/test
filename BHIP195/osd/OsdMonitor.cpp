#include "OsdMonitor.h"
#include "OSTime.h"

#include <iconv.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <string.h>

#define GB2312_TO_UNICODE       			0
#define UNICODE_TO_GB2312       			1

static unsigned char g_latticeKey[8] = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};

#if 0
char *	g_dataHZK16 = NULL;
char *	g_dataHZK48 = NULL;
char *	g_dataASC16 = NULL;
char *	g_dataASC48 = NULL;
#endif

enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };

COsdMonitor::COsdMonitor(void)
: m_ullOsdTm(0)
, m_bReInit(false)
, m_nFontSize(0)
, m_nPosition(0)
, m_nAntiColor(0)
, m_nAlign(0)
, m_nType(0)
, m_nX(0)
, m_nY(0)
, m_nWidth(0)
, m_nHeight(0)
, m_nFrameRate(0)
, m_nTimeWidth (0)
, m_nTimeHeight(0)
, m_nTextWidth(0)
, m_nTextHeight(0)
, m_pFrameBufferIn(NULL)
, m_pFrameBufferOut(NULL)
, m_pInFrame(NULL)
, m_pOutFrame(NULL)
, m_pBufferSrc(NULL)
, m_pBufferSink(NULL)
, m_pBufferSrcCtx(NULL)
, m_pBufferSinkCtx(NULL)
, m_pFilterGraph(NULL)
, m_pOutputs(NULL)
, m_pInputs(NULL)
{	
	m_strVideoAlarmDesc = m_strAudioAlarmDesc = "";
	//m_strVideoAlarmDescLast = m_strAudioAlarmDescLast = "";
    m_strAlarmFilter = m_strAlarmBase = "";
}

void COsdMonitor::InitFilter(const char * filter_desc)
{
    if (m_pFilterGraph)
    {
        avfilter_graph_free(&m_pFilterGraph);
        m_pFilterGraph = NULL;
    }

    avfilter_register_all();

    m_pBufferSrc = avfilter_get_by_name("buffer");
    m_pBufferSink = avfilter_get_by_name("buffersink");

    m_pOutputs = avfilter_inout_alloc();
    m_pInputs = avfilter_inout_alloc();

    m_pFilterGraph = avfilter_graph_alloc();

    char args[512];
    snprintf(args, sizeof(args),  
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",  
        m_nWidth, m_nHeight, AV_PIX_FMT_YUV420P,  
        1, m_nFrameRate, 1, 1); 

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    int ret = avfilter_graph_create_filter(&m_pBufferSrcCtx, m_pBufferSrc, "in",
        args, NULL, m_pFilterGraph);
    if (ret < 0) {  
        log_error("Cannot create buffer source");
        return;  
    }

    /* buffer video sink: to terminate the filter chain. */
    AVBufferSinkParams * pBufferSinkParams = av_buffersink_params_alloc();
    pBufferSinkParams->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&m_pBufferSinkCtx, m_pBufferSink, "out",
        NULL, pBufferSinkParams, m_pFilterGraph);
    av_free(pBufferSinkParams);
    if (ret < 0) {
        log_error("Cannot create buffer sink");
        return;
    }

    /* Endpoints for the filter graph. */  
    m_pOutputs->name = av_strdup("in");  
    m_pOutputs->filter_ctx = m_pBufferSrcCtx;  
    m_pOutputs->pad_idx = 0;  
    m_pOutputs->next = NULL;  

    m_pInputs->name = av_strdup("out");  
    m_pInputs->filter_ctx = m_pBufferSinkCtx;
    m_pInputs->pad_idx = 0;
    m_pInputs->next = NULL;

    if ((ret = avfilter_graph_parse_ptr(m_pFilterGraph, (const char *)filter_desc,
        &m_pInputs, &m_pOutputs, NULL)) < 0)  
    {
        log_error("avfilter_graph_parse_ptr error");
        return;  
    }

    if ((ret = avfilter_graph_config(m_pFilterGraph, NULL)) < 0)
    {
        log_error("avfilter_graph_config error");
        return;  
    }

    avfilter_inout_free(&m_pInputs);
    avfilter_inout_free(&m_pOutputs);
}

void COsdMonitor::Init(int nW, int nH, int nFrameRate)
{
    m_nWidth = nW;
    m_nHeight = nH;
    m_nFrameRate = nFrameRate;

    int nFontSize = 0;

    if (m_nWidth > 1920)
    {
        nFontSize = 120;
    }
    else if (m_nWidth > 960)
    {
        nFontSize = 80;
    }
    else if (m_nWidth > 352)
    {
        nFontSize = 60;
    }
    else
    {
        nFontSize = 40;
    }
    
    char strBase[128];
    snprintf(strBase, sizeof(strBase), "drawtext=fontfile=/usr/share/fonts/chinese/simsun.ttc:fontcolor=red:fontsize=%d", nFontSize);

    m_strAlarmBase = string(strBase, strlen(strBase));

#if 0
    avfilter_register_all();

    m_pBufferSrc = avfilter_get_by_name("buffer");
    m_pBufferSink = avfilter_get_by_name("buffersink");

    m_pOutputs = avfilter_inout_alloc();
    m_pInputs = avfilter_inout_alloc();

    m_pFilterGraph = avfilter_graph_alloc();

    char args[512];
    snprintf(args, sizeof(args),  
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",  
        m_nWidth, m_nHeight, AV_PIX_FMT_YUV420P,  
        1, 25, 1, 1); 

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    int ret = avfilter_graph_create_filter(&m_pBufferSrcCtx, m_pBufferSrc, "in",
        args, NULL, m_pFilterGraph);
    if (ret < 0) {  
        log_error("Cannot create buffer source");
        return;  
    }

    /* buffer video sink: to terminate the filter chain. */
    AVBufferSinkParams * pBufferSinkParams = av_buffersink_params_alloc();
    pBufferSinkParams->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&m_pBufferSinkCtx, m_pBufferSink, "out",
        NULL, pBufferSinkParams, m_pFilterGraph);
    av_free(pBufferSinkParams);
    if (ret < 0) {
        log_error("Cannot create buffer sink");
        return;
    }

    /* Endpoints for the filter graph. */  
    m_pOutputs->name = av_strdup("in");  
    m_pOutputs->filter_ctx = m_pBufferSrcCtx;  
    m_pOutputs->pad_idx = 0;  
    m_pOutputs->next = NULL;  

    m_pInputs->name = av_strdup("out");  
    m_pInputs->filter_ctx = m_pBufferSinkCtx;
    m_pInputs->pad_idx = 0;
    m_pInputs->next = NULL;

    char filter_desc[512];
    snprintf(filter_desc, sizeof(filter_desc), "drawtext=fontfile=/usr/share/fonts/Arial/arial.ttf:fontcolor=red:fontsize=30:text='%s':x=50:y=50", GenerateTimeForOsd(OS_WallTime_Ms()).c_str());

    //const char * filter_desc = "drawtext=fontcolor=red:fontsize=30:text='zhanghui':x=50:y=50";

    if ((ret = avfilter_graph_parse_ptr(m_pFilterGraph, (const char *)filter_desc,
        &m_pInputs, &m_pOutputs, NULL)) < 0)  
    {
        log_error("avfilter_graph_parse_ptr error");
        return;  
    }

    if ((ret = avfilter_graph_config(m_pFilterGraph, NULL)) < 0)
    {
        log_error("avfilter_graph_config error");
        return;  
    }
#endif

    m_pInFrame = av_frame_alloc();
    m_pFrameBufferIn = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, nW, nH, 1));
    av_image_fill_arrays(m_pInFrame->data, m_pInFrame->linesize, m_pFrameBufferIn,
        AV_PIX_FMT_YUV420P, nW, nH, 1);

    m_pOutFrame = av_frame_alloc();
    m_pFrameBufferOut = (unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, nW, nH, 1));
    av_image_fill_arrays(m_pOutFrame->data, m_pOutFrame->linesize, m_pFrameBufferOut,
        AV_PIX_FMT_YUV420P, nW, nH, 1);

    m_pInFrame->width = nW;
    m_pInFrame->height = nH;
    m_pInFrame->format = AV_PIX_FMT_YUV420P;

    log_info("OSD monitor init success");
}

void COsdMonitor::DeInit()
{
    av_frame_free(&m_pInFrame);  
    av_frame_free(&m_pOutFrame);  

    avfilter_graph_free(&m_pFilterGraph);

    avfilter_inout_free(&m_pInputs);
    avfilter_inout_free(&m_pOutputs);
}

void COsdMonitor::SetOsdInfo(OsdInfo & stOSD)
{
    m_nFontSize = stOSD.nFontSize;
    m_nPosition = stOSD.nPosition;
    m_nAntiColor = stOSD.nAntiColor;
    m_nAlign = stOSD.nAlign;
    m_nType = stOSD.nType;

    char strBase[128];
    snprintf(strBase, sizeof(strBase), "drawtext=fontfile=/usr/share/fonts/chinese/simsun.ttc:fontcolor=0xc0c0c0:fontsize=%d"
        , m_nFontSize
        );
 
    m_strBase = string(strBase, strlen(strBase));

#if 1
    m_strOsdText = stOSD.strText;
#else
    if (m_strOsdText != stOSD.strText)
    {
        m_strOsdText = stOSD.strText;

        OsdTextTransform(m_strOsdText.c_str(), m_strOsdText.length(), m_nFontSize, &m_yuvTextInfo);
        m_nTextWidth = m_yuvTextInfo.width;
        m_nTextHeight = m_yuvTextInfo.height;
    }
#endif

    m_bReInit = true;
}

#if 0
int COsdMonitor::OsdTextTransform(const char * word, int inLen, unsigned int fontSize, OSD_TEXT_YUV_LATTICE * yuvInfo)
{
	int i, j, k, offset;	
	int value;
	char *pBuffer;

	fontSize = 16;

	if (g_dataHZK16 == NULL
		|| g_dataHZK48 == NULL
		|| g_dataASC16 == NULL
		|| g_dataASC48 == NULL)
	{
		return 0;
	}		

	if (fontSize != 16 && fontSize != 48)
	{
		log_error("not support this lattice font size: %d", fontSize);
		return -1;
	}

	if (yuvInfo->yAddr == NULL)
	{
		log_info("Y addr can not be NULL !!\r\n");
		return -2;		
	}

	//取点
	int nPos = 0;
	yuvInfo->width = 0;
	yuvInfo->height = fontSize;
	yuvInfo->stride = OSD_TEXT_LATTICE_STRIDE;

	while (1)
	{
		if(word[nPos] == 0 || nPos >= inLen) break;		
		
		if ((unsigned char)word[nPos] > 127)	//中文
		{
			if (fontSize == 16)	//16*16
			{
				offset = (94 * (unsigned char)(word[nPos] - 0xa0 - 1) + (unsigned char)(word[nPos + 1] - 0xa0 - 1)) * 32;
				pBuffer = (g_dataHZK16 + offset);			

				for (k = 0; k < 16; k++)
					for (j = 0; j < 2; j++)
						for(i = 0; i < 8; i++)								
							yuvInfo->yAddr[yuvInfo->width + k * OSD_TEXT_LATTICE_STRIDE + j * 8 + i] = pBuffer[k * 2 + j] & g_latticeKey[i];						
			}
			else if (fontSize == 48)	//48*48
			{
				offset = (94 * (unsigned int)(word[nPos] - 0xa0 - 1) + (word[nPos + 1] - 0xa0 - 1)) * 288;
				pBuffer = g_dataHZK48 + offset;

				for (k = 0; k < 48; k++)
					for (j = 0; j < 6; j++)
						for (i = 0; i < 8; i++)			       
							yuvInfo->yAddr[yuvInfo->width + k * OSD_TEXT_LATTICE_STRIDE + j * 8 + i] = pBuffer[k * 6 + j] & g_latticeKey[i];						       
			}

			nPos += 2;
			yuvInfo->width += fontSize;
		}
		else		//ASCII字符
		{
			if (fontSize == 16)	//8*16
			{
				offset = word[nPos] * 16;
				pBuffer = g_dataASC16 + offset;		

				for ( k = 0; k < 16; k++)
					for (i = 0; i < 8; i++)
						yuvInfo->yAddr[yuvInfo->width + k * OSD_TEXT_LATTICE_STRIDE + i] = pBuffer[k] & g_latticeKey[i];
			}
			else if(fontSize == 48)	//24*48
			{
				offset = word[nPos] * 144;
				pBuffer = g_dataASC48 + offset;		

				for (k = 0; k < 48; k++)
					for (j = 0; j < 3; j++)
						for (i = 0; i < 8; i++)			       
							yuvInfo->yAddr[yuvInfo->width + k * OSD_TEXT_LATTICE_STRIDE + j * 8 + i] = pBuffer[k * 3 + j] & g_latticeKey[i];						       
			}

			nPos++;			
			yuvInfo->width += (fontSize / 2);
		}
	}

	//转换成YUV 420 数据格式
	for ( i = 0; i < (int)fontSize; i++)			       
	{
		for(j=0; j<yuvInfo->width; j++)
		{
			value = yuvInfo->yAddr[i * OSD_TEXT_LATTICE_STRIDE+j];

			yuvInfo->yAddr[i * OSD_TEXT_LATTICE_STRIDE + j] = (value ? 255 : 0);
		}		
	}

	return 0;
}

void COsdMonitor::SetTextPosition(int nInHeight, int nInPicth, int textWidth, int textHeight, int timerWidth, int timerHeight)
{
	if (0 == m_nPosition)
	{
		m_nX = 0;
		m_nY = 0;
	}
	else if (1 == m_nPosition)
	{
		if (1 == m_nType || 3 == m_nType)
		{
			if (1 == m_nType)
			{
				m_nX = nInPicth - (textWidth > timerWidth ? textWidth : timerWidth);
				if (m_nX < 0)
				{
					m_nX = 0;
				}
			}
			else
			{
				m_nX = nInPicth - textWidth - timerWidth;
				if (m_nX < 0)
				{
					m_nX = 0;
				}
			}
		}
		else if (2 == m_nType)
		{
			m_nX = nInPicth - textWidth;
			if (m_nX < 0)
			{
				m_nX = 0;
			}
		}
		m_nY = 0;
	}
	else if (2 == m_nPosition)
	{
		m_nX = 0;
		if (1 == m_nType )
		{
			m_nY = nInHeight - textHeight-timerHeight;
			if (m_nY < 0)
			{
				m_nY = 0;
			}
		}
		else if (2 == m_nType || 3 == m_nType)
		{
			m_nY = nInHeight - textHeight;
			if (m_nY < 0)
			{
				m_nY = 0;
			}
		}

	}
	else if (3 == m_nPosition)
	{
		if (1 == m_nType || 3 == m_nType)
		{			
			if (1 == m_nType)
			{
				m_nX = nInPicth - (textWidth > timerWidth ? textWidth : timerWidth);
				if (m_nX < 0)
				{
					m_nX = 0;
				}
				m_nY = nInHeight - textHeight-timerHeight;
				if (m_nY < 0)
				{
					m_nY = 0;
				}
			}
			else
			{
				m_nX = nInPicth - textWidth - timerWidth;
				if (m_nX < 0)
				{
					m_nX = 0;
				}
				m_nY = nInHeight - textHeight;
				if (m_nY < 0)
				{
					m_nY = 0;
				}
			}
			
		}
		else if (2 == m_nType)
		{
			m_nX = nInPicth - textWidth;
			if (m_nX < 0)
			{
				m_nX = 0;
			}
			m_nY = nInHeight - textHeight;
			if (m_nY < 0)
			{
				m_nY = 0;
			}
		}
	}
}


int COsdMonitor::DrawOsd(unsigned char *pYAddr, int nWidth, int nHeight, int nPitch)
{
    if ( ! m_strOsdText.empty())
    {
        if (1 == m_nType || 3 == m_nType)			//判断是否需要加入时间
        {
            unsigned long long ullCurTm = OS_WallTime_Ms();
            if (m_ullOsdTm == 0 || ullCurTm - m_ullOsdTm > 1000 )
            {
                OsdTextTransform(GenerateReportTime(ullCurTm).c_str(), GenerateReportTime(ullCurTm).length(), m_nFontSize, &m_yuvTimerInfo);
                m_nTimeWidth = m_yuvTimerInfo.width;
                m_nTimeHeight = m_yuvTimerInfo.height;
                m_ullOsdTm = ullCurTm;
            }
        }

        if (! m_strVideoAlarmDesc.empty())
        {
            if (strcmp(m_strVideoAlarmDescLast.c_str(), m_strVideoAlarmDesc.c_str()))
            {
                m_strVideoAlarmDescLast = m_strVideoAlarmDesc;
                OsdTextTransform(m_strVideoAlarmDescLast.c_str(), m_strVideoAlarmDescLast.size()-1, m_nFontSize, &m_yuvVideoMonitorInfo);
            } 
        }

        if (! m_strAudioAlarmDesc.empty())
        {
            if (strcmp(m_strAudioAlarmDescLast.c_str(), m_strAudioAlarmDesc.c_str()))
            {
                m_strAudioAlarmDescLast = m_strAudioAlarmDesc;
                OsdTextTransform(m_strAudioAlarmDescLast.c_str(), m_strAudioAlarmDescLast.size()-1, m_nFontSize, &m_yuvAudioMonitorInfo);
            }
        }

        int i = 0;

        //0-左上角；1-右上角；2-左下角；3-右下角
        SetTextPosition(nHeight, nPitch, m_nTextWidth, m_nTextHeight, m_nTimeWidth, m_nTimeHeight);

        for (i = m_nY; i < (m_nY + m_nTextHeight); i++)		
        {
            memcpy(pYAddr + m_nX+ i* nPitch
                , m_yuvTextInfo.yAddr + (i - m_nY) * (m_yuvTextInfo.stride)
                , m_nTextWidth);
        }

        if (1 == m_nType || 3 == m_nType)
        {
            if (1 == m_nType)
            {
                m_nY += m_nTextHeight;
            }
            else
            {
                m_nX += m_nTextWidth;
            }
            for(i = m_nY; i < (m_nY+m_nTimeHeight); i++)		
            {
                memcpy(pYAddr + m_nX+ i* nPitch
                    , m_yuvTimerInfo.yAddr + (i-m_nY)*(m_yuvTimerInfo.stride)
                    , m_nTimeWidth);			
            }

            if (1 == m_nType)
            {
                m_nY += m_nTimeHeight;
            }
            else if (2 == m_nType || 3 == m_nType)
            {
                m_nY += m_nTextHeight;
            }

            for (i = m_nY; i < (m_nY + m_yuvVideoMonitorInfo.height); i++)
            {
                memcpy(pYAddr + m_nX + i* nPitch
                    , m_yuvVideoMonitorInfo.yAddr + (i - m_nY)*(m_yuvVideoMonitorInfo.stride)
                    , m_yuvVideoMonitorInfo.width);
            }

            m_nY += m_yuvVideoMonitorInfo.height;
            for (i = m_nY; i < (m_nY + m_yuvAudioMonitorInfo.height); i++)
            {
                memcpy(pYAddr + m_nX + i* nPitch
                    , m_yuvAudioMonitorInfo.yAddr + (i - m_nY)*(m_yuvAudioMonitorInfo.stride)
                    , m_yuvAudioMonitorInfo.width);
            }
        }
    }

    return 1;
}
#endif

void COsdMonitor::ReInitFilterDesc()
{
    char filter_desc[512];

    unsigned long long ullCurTm = OS_WallTime_Ms();
    string strCurTm = GenerateTimeForOsd(ullCurTm);
    int nCurTmLen = strCurTm.length() - 2;

    // Position(0-左上角；1-右上角；2-左下角；3-右下角)
    switch (m_nPosition)
    {
    case 0:
        if (m_nType == 1)
        {
            // Align(0-左对齐；1-居中；2-右对齐)
            switch (m_nAlign)
            {
            case 0:
                snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=5", m_strBase.c_str());
                break;
            case 1:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)(m_strOsdText.length() - nCurTmLen);
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=5:text='%s',%s:x=tw*%d/%d+5:y=5+2*th"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff / 2
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=tw*%d/%d+5:y=5:text='%s',%s:x=5:y=5+2*th"
                            , m_strBase.c_str()
                            , ndiff / 2
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            case 2:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=5:text='%s',%s:x=tw*%d/%d+5:y=5+2*th"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=tw*%d/%d+5:y=5:text='%s',%s:x=5:y=5+2*th"
                            , m_strBase.c_str()
                            , ndiff
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            default:
                log_error("Unsupported align mode");
                break;
            }
        }
        else
        {
            snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=5", m_strBase.c_str());
        }
        break;
    case 1: // Position(0-左上角；1-右上角；2-左下角；3-右下角)
        if (m_nType == 1)
        {
            // Align(0-左对齐；1-居中；2-右对齐)
            switch (m_nAlign)
            {
            case 0:
                snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=5", m_strBase.c_str());
                break;
            case 1:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=5:text='%s',%s:x=w-tw-5-tw*%d/%d:y=5+2*th"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff / 2
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5-tw*%d/%d:y=5:text='%s',%s:x=w-tw-5:y=5+2*th"
                            , m_strBase.c_str()
                            , ndiff / 2
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            case 2:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=5:text='%s',%s:x=w-tw-5:y=5+2*th"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=5:text='%s',%s:x=w-tw-5:y=5+2*th"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            default:
                log_error("Unsupported align mode");
                break;
            }
        }
        else
        {
            snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=5", m_strBase.c_str());
        }
        break;
    case 2: // Position(0-左上角；1-右上角；2-左下角；3-右下角)
        if (m_nType == 1)
        {
            // Align(0-左对齐；1-居中；2-右对齐)
            switch (m_nAlign)
            {
            case 0:
                snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=h-th-5", m_strBase.c_str());
                break;
            case 1:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=h-3*th-5:text='%s',%s:x=5+tw*%d/%d:y=h-th-5"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff / 2
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5+tw*%d/%d:y=h-3*th-5:text='%s',%s:x=5:y=h-th-5"
                            , m_strBase.c_str()
                            , ndiff / 2
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            case 2:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=h-3*th-5:text='%s',%s:x=5+tw*%d/%d:y=h-th-5"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=5+tw*%d/%d:y=h-3*th-5:text='%s',%s:x=5:y=h-th-5"
                            , m_strBase.c_str()
                            , ndiff
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            default:
                log_error("Unsupported align mode");
                break;
            }
        }
        else
        {
            snprintf(filter_desc, sizeof(filter_desc), "%s:x=5:y=h-th-5", m_strBase.c_str());
        }
        break;
    case 3: // Position(0-左上角；1-右上角；2-左下角；3-右下角)
        if (m_nType == 1)
        {
            // Align(0-左对齐；1-居中；2-右对齐)
            switch (m_nAlign)
            {
            case 0:
                snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=h-th-5", m_strBase.c_str());
                break;
            case 1:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=h-3*th-5:text='%s',%s:x=w-tw-5-tw*%d/%d:y=h-th-5"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            , ndiff / 2
                            , nCurTmLen
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5-tw*%d/%d:y=h-3*th-5:text='%s',%s:x=w-tw-5:y=h-th-5"
                            , m_strBase.c_str()
                            , ndiff / 2
                            , (int)m_strOsdText.length()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            case 2:
                {
                    if ((int)m_strOsdText.length() > nCurTmLen)
                    {
                        int ndiff = (int)m_strOsdText.length() - nCurTmLen;
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=h-3*th-5:text='%s',%s:x=w-tw-5:y=h-th-5"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                    else
                    {
                        int ndiff = nCurTmLen - (int)m_strOsdText.length();
                        snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=h-3*th-5:text='%s',%s:x=w-tw-5:y=h-th-5"
                            , m_strBase.c_str()
                            , m_strOsdText.c_str()
                            , m_strBase.c_str()
                            );
                    }
                }
                break;
            default:
                log_error("Unsupported align mode");
                break;
            }
        }
        else
        {
            snprintf(filter_desc, sizeof(filter_desc), "%s:x=w-tw-5:y=h-th-5", m_strBase.c_str());
        }
        break;
    default:
        log_error("Unsupported position set");
        break;
    }

    m_strBase = string(filter_desc, strlen(filter_desc));

    // Type(1-第一行为text，第二行为时间信息。2-第一行为text，无时间信息。3-text和时间信息在一行显示)
    if (m_nType == 2)
    {
        snprintf(filter_desc, sizeof(filter_desc), "%s:text='%s'", m_strBase.c_str(), m_strOsdText.c_str());
        InitFilter(filter_desc);
    }
}

void COsdMonitor::DrawOsdUseFfmpeg(mfxFrameSurface1 *pSurface)
{
    if (m_bReInit)
    {
        m_bReInit = false;
        ReInitFilterDesc();
    }

    char filter_desc[1024];

    unsigned long long ullCurTm = OS_WallTime_Ms();
    string strCurTm = GenerateTimeForOsd(ullCurTm);
    int nCurTmLen = strCurTm.length() - 2;

    if (m_ullOsdTm == 0 || ullCurTm - m_ullOsdTm > 1000)
    {
        bool bTimeReInit = false;

        if (m_nType == 1)
        {
            // Align(0-左对齐；1-居中；2-右对齐)
            switch (m_nAlign)
            {
            case 0:
                snprintf(filter_desc, sizeof(filter_desc), "%s:text='%s\n\n%s'", m_strBase.c_str(), m_strOsdText.c_str(), strCurTm.c_str());
                break;
            case 1:
                snprintf(filter_desc, sizeof(filter_desc), "%s:text='%s'", m_strBase.c_str(), strCurTm.c_str());
                break;
            case 2:
                snprintf(filter_desc, sizeof(filter_desc), "%s:text='%s'", m_strBase.c_str(), strCurTm.c_str());
                break;
            default:
                log_error("Unsupported align mode");
                break;
            }

            bTimeReInit = true;
        }
        else if (m_nType == 3)
        {
            snprintf(filter_desc, sizeof(filter_desc), "%s:text='%s %s'", m_strBase.c_str(), m_strOsdText.c_str(), strCurTm.c_str());
            bTimeReInit = true;
        }

        char filter_alarm[512];

        if (m_strVideoAlarmDesc.length() && m_strAudioAlarmDesc.length())
        {
            snprintf(filter_alarm, sizeof(filter_alarm), "%s:x=(w-tw)/2:y=h/2-th-5:text='%s',%s:x=(w-tw)/2:y=h/2+5:text='%s'"
                , m_strAlarmBase.c_str(), m_strVideoAlarmDesc.c_str()
                , m_strAlarmBase.c_str(), m_strAudioAlarmDesc.c_str()
                );
            m_strAlarmFilter = string(filter_alarm, strlen(filter_alarm));
        }
        else if (m_strAudioAlarmDesc.length() && m_strVideoAlarmDesc.empty())
        {
            snprintf(filter_alarm, sizeof(filter_alarm), "%s:x=(w-tw)/2:y=(h-th)/2:text='%s'", m_strAlarmBase.c_str(), m_strAudioAlarmDesc.c_str());
            m_strAlarmFilter = string(filter_alarm, strlen(filter_alarm));
        }
        else if (m_strAudioAlarmDesc.empty() && m_strVideoAlarmDesc.length())
        {
            snprintf(filter_alarm, sizeof(filter_alarm), "%s:x=(w-tw)/2:y=(h-th)/2:text='%s'", m_strAlarmBase.c_str(), m_strVideoAlarmDesc.c_str());
            m_strAlarmFilter = string(filter_alarm, strlen(filter_alarm));
        }
        else
        {
            m_strAlarmFilter = "";
        }
        
        if (bTimeReInit || m_strAlarmFilter.length())
        {
            if (m_strAlarmFilter.length())
            {
                string strTemp = string(filter_desc, strlen(filter_desc));
                snprintf(filter_desc, sizeof(filter_desc), "%s,%s", strTemp.c_str(), m_strAlarmFilter.c_str());
            }

            InitFilter(filter_desc);
        }

        m_ullOsdTm = ullCurTm;
    }

    mfxFrameInfo &pInfo = pSurface->Info;
    mfxFrameData &pData = pSurface->Data;

    mfxU32 i, j, h, w, pitch;

    unsigned char * pY = m_pFrameBufferIn;
    unsigned char * pU = m_pFrameBufferIn + m_nWidth * m_nHeight;
    unsigned char * pV = m_pFrameBufferIn + m_nWidth * m_nHeight * 5 / 4;

    switch (pInfo.FourCC)
    {
    case MFX_FOURCC_YV12:
    case MFX_FOURCC_NV12:
        for (i = 0; i < pInfo.CropH; i++)
        {
            memcpy(pY,
                pData.Y + (pInfo.CropY * pData.Pitch + pInfo.CropX)+ i * pData.Pitch,
                pInfo.CropW);
            pY += pInfo.CropW;
        }
        break;
    default:
        break;
    }

    switch (pInfo.FourCC)
    {
    case MFX_FOURCC_NV12:

        h = pInfo.CropH / 2;
        w = pInfo.CropW;
        for (i = 0; i < h; i++)
        {
            for (j = 0; j < w; j += 2)
            {
                memcpy(pU++,
                    pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX) + i * pData.Pitch + j,
                    1);
            }
        }

        for (i = 0; i < h; i++)
        {
            for (j = 1; j < w; j += 2)
            {
                memcpy(pV++,
                    pData.UV + (pInfo.CropY * pData.Pitch / 2 + pInfo.CropX)+ i * pData.Pitch + j,
                    1);
            }
        }
        break;
    default:
        break;
    }

    m_pInFrame->data[0] = m_pFrameBufferIn;  
    m_pInFrame->data[1] = m_pFrameBufferIn + m_nWidth * m_nHeight;  
    m_pInFrame->data[2] = m_pFrameBufferIn + m_nWidth * m_nHeight * 5 / 4;

#if 0
    static FILE * fp_in = fopen("in.yuv", "wb+");
    if (fp_in)
    {
        fwrite(m_pFrameBufferIn, 1, m_nWidth * m_nHeight * 3 * sizeof(unsigned char) / 2, fp_in);
    }
#endif

    if (av_buffersrc_add_frame(m_pBufferSrcCtx, m_pInFrame) < 0)
    {
        log_error("Error while add frame.\n");
        return;
    }  

    /* pull filtered pictures from the filtergraph */  
    int ret = av_buffersink_get_frame(m_pBufferSinkCtx, m_pOutFrame);
    if (ret < 0) return;

    if (m_pOutFrame->format == AV_PIX_FMT_YUV420P)
    {
#if 0
        static FILE * fp_out = fopen("out.yuv", "wb+");
        if (fp_out)
        {
            for (int i = 0; i<m_pOutFrame->height; i++) {  
                fwrite(m_pOutFrame->data[0] + m_pOutFrame->linesize[0] * i, 1, m_pOutFrame->width, fp_out);  
            }  
            for (int i = 0; i<m_pOutFrame->height / 2; i++) {  
                fwrite(m_pOutFrame->data[1] + m_pOutFrame->linesize[1] * i, 1, m_pOutFrame->width / 2, fp_out);  
            }  
            for (int i = 0; i<m_pOutFrame->height / 2; i++) {  
                fwrite(m_pOutFrame->data[2] + m_pOutFrame->linesize[2] * i, 1, m_pOutFrame->width / 2, fp_out);  
            }
        }
#endif

        if (pInfo.CropH > 0 && pInfo.CropW > 0)
        {
            w = pInfo.CropW;
            h = pInfo.CropH;
        }
        else
        {
            w = pInfo.Width;
            h = pInfo.Height;
        }

        mfxU8 *ptr, *ptr2;

        pitch = pData.Pitch;
        ptr = pData.Y + pInfo.CropX + pInfo.CropY * pData.Pitch;

        // read luminance plane
        for (i = 0; i < h; i++)
        {
            memcpy(ptr + i * pitch, m_pOutFrame->data[0] + m_pOutFrame->linesize[0] * i, w);
        }

        switch (pInfo.FourCC)
        {
        case MFX_FOURCC_NV12:
            mfxU8 buf[2048]; // maximum supported chroma width for nv12
            mfxU32 j;
            w /= 2;
            h /= 2;
            ptr = pData.UV + pInfo.CropX + (pInfo.CropY / 2) * pitch;
            if (w > 2048)
            {
                break;
            }
            // load U
            for (i = 0; i < h; i++)
            {
                ptr2 = m_pOutFrame->data[1] + m_pOutFrame->linesize[1] * i;

                for (j = 0; j < w; j++)
                {
                    ptr[i * pitch + j * 2] = ptr2[j];
                }
            }
            // load V
            for (i = 0; i < h; i++)
            {
                ptr2 = m_pOutFrame->data[2] + m_pOutFrame->linesize[2] * i;

                for (j = 0; j < w; j++)
                {
                    ptr[i * pitch + j * 2 + 1] = ptr2[j];
                }
            }

            break;
        case MFX_FOURCC_YV12:
            w /= 2;
            h /= 2;
            pitch /= 2;

            ptr  = pData.U + (pInfo.CropX / 2) + (pInfo.CropY / 2) * pitch;
            ptr2 = pData.V + (pInfo.CropX / 2) + (pInfo.CropY / 2) * pitch;

            for(i = 0; i < h; i++)
            {
                memcpy(ptr + i * pitch, m_pOutFrame->data[1] + m_pOutFrame->linesize[1] * i, w);
            }
            for(i = 0; i < h; i++)
            {
                memcpy(ptr2 + i * pitch, m_pOutFrame->data[2] + m_pOutFrame->linesize[2] * i, w);
            }

            break;
        default:
            break;
        }
    }

    av_frame_unref(m_pOutFrame);  
}


COsdMonitor::~COsdMonitor(void)
{
}

int COsdMonitor::CatchVideoMonitor(unsigned short VideoMonitor)
{
	if (0 != VideoMonitor)
	{
        string strDesc = "";
	    unsigned short sWarnVideoStatus = VideoMonitor;
	    int nIndex = 0;
	    while (sWarnVideoStatus)
	    {
	        if ((sWarnVideoStatus & 0x01) == 1)
	        {
				switch (nIndex)
				{
				case WARN_TYPE_V_BLACK:
					strDesc += "黑场";
					break;
				case WARN_TYPE_V_STILL:
					strDesc += "静帧";
					break;
				case WARN_TYPE_V_COLORBAR:
					strDesc += "彩条";
					break;
				case WARN_TYPE_V_COLORFRAME:
					strDesc += "彩场";
					break;
				case WARN_TYPE_V_CLORLFRAME_RED:
					strDesc += "红场";
					break;
				case WARN_TYPE_V_CLORLFRAME_BLUE:
					strDesc += "蓝场";
					break;
				case WARN_TYPE_V_CLORLFRAME_GREEN:
					strDesc += "绿场";
					break;
				case WARN_TYPE_V_CLORLFRAME_GRAY:
					strDesc += "灰场";
					break;
				case WARN_TYPE_V_CLORLFRAME_WHITE:
					strDesc += "白场";
					break;
				case WARN_TYPE_V_DEC:
					strDesc += "视频解码异常";
					break;
				case WARN_TYPE_V_LOST:
					strDesc += "视频丢失";
					break;
				case WARN_TYPE_V_SCRAMB:
					strDesc += "视频解扰异常";
					break;
				case WARN_TYPE_V_TEST:
					strDesc += "测试卡";
					break;
				case WARN_TYPE_V_MAX:
					strDesc += "报警类型数量最大值";
					break;
				default:
					break;
				}
                if (strDesc.length())
				    strDesc += " ";
	        }
	        sWarnVideoStatus >>= 1;
	        nIndex++;
	    }

        m_strVideoAlarmDesc = strDesc;
    }
	else
	{
#if 0
		m_yuvVideoMonitorInfo.height = 0;
		m_yuvVideoMonitorInfo.stride = 0;
		m_yuvVideoMonitorInfo.width = 0;
		m_strVideoAlarmDescLast = "";
#endif
        m_strVideoAlarmDesc = "";
	}

	return 0;
}

int COsdMonitor::CatchAudioMonitor(unsigned short AudioMonitor)
{			
	if (0 != AudioMonitor)
	{
        string strDesc = "";
		unsigned short sWarnAudioStatus = AudioMonitor;
		int nIndex = 0;
		while (sWarnAudioStatus)
		{
			if ((sWarnAudioStatus & 0x01) == 1)
			{
				switch (nIndex)
				{
				case WARN_TYPE_A_LOST: 
					strDesc += "音频丢失";
					break;
				case WARN_TYPE_A_NO_SOUND:      
					strDesc += "音频无伴音";
					break;
				case WARN_TYPE_A_MAX:         
					strDesc += "报警类型数量最大值";
					break;
				default:
					break;
				}
				if (strDesc.length()) strDesc += " ";
			}
			sWarnAudioStatus >>= 1;
			nIndex++;
		}

        m_strAudioAlarmDesc = strDesc;
	}
    else
    {
        m_strAudioAlarmDesc = "";
    }
	
	return 0;
}