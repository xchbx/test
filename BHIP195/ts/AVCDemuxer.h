// AVCDemuxer.h: interface for the CAVCDemuxer class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AVCDEMUXER_H__C1E93114_5E05_4276_B473_5AE861F7B495__INCLUDED_)
#define AFX_AVCDEMUXER_H__C1E93114_5E05_4276_B473_5AE861F7B495__INCLUDED_

typedef unsigned char byte;

// 视频信息结构体
struct VParam
{
	int i_width;			// 视频宽
	int i_height;			// 视频高

	VParam(int w = 0, int h = 0) : i_width(w), i_height(h){}
};

struct SliceCount
{
    int i_I_frame_cnt;
    int i_P_frame_cnt;
    int i_B_frame_cnt;

    SliceCount() : i_I_frame_cnt(0), i_P_frame_cnt(0), i_B_frame_cnt(0) {}
};

/************************************************************************
	函数：Getx264Info
	说明：从给定的数据中获取AVC信息
	参数：p_payload 要处理的数据
		  i_payload 数据长度
		  info		返回数据信息
	返回值：-1 失败，其它成功
	作者：wutong
	时间：2010-04-07
************************************************************************/
int     Getx264Info( byte* p_payload, int i_payload, VParam & info );

// 获取帧类型
int     Getx264SliceCount( byte* p_payload, int i_payload, SliceCount & s_slice_cnt );

/*******************************************************************************
* 函数名称：	GetMpeg2Info
* 功能描述：	从给定的数据中获取MPEG2参数信息
* 输入参数：	
* 输出参数：	
* 返 回 值：	
* 其它说明：	
* 修改日期		修改人	      修改内容
* ------------------------------------------------------------------------------
* 2013-05-16 	张家斌	      创建
*******************************************************************************/
int		GetMpeg2Info( byte* p_payload, int i_payload, VParam& info );


#endif // !defined(AFX_AVCDEMUXER_H__C1E93114_5E05_4276_B473_5AE861F7B495__INCLUDED_)
