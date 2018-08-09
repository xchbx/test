/*
 * Chinese AVS video (AVS1-P2, JiZhun profile) decoder.
 * Copyright (c) 2006  Stefan Gehrer <stefan.gehrer@gmx.de>
 *
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
/*************************************************************
chengdu WP 20070526 - 20070730
secondwang@163.com
xavs解码器2.0版本

**************************************************************/

#ifndef _XAVS_GLOBE_H
#define _XAVS_GLOBE_H

#include <memory.h>
#include <math.h>
#include <limits.h>
extern uint8_t crop_table[];
extern const xavs_vector MV_NOT_AVAIL;
extern const xavs_vector MV_REF_DIR;
extern const xavs_vector MV_INTRA;
extern const xavs_mvref MVREF_NOT_AVAIL;
extern const uint8_t dequant_shift[64];
extern const uint16_t dequant_mul[64];
extern const uint8_t zigzag_progressive[64];
extern const uint8_t zigzag_field[64];
extern const uint8_t chroma_qp[64] ;
extern const xavs_vlc intra_2dvlc[7];
extern const xavs_vlc inter_2dvlc[7];
extern const xavs_vlc chroma_2dvlc[5];
extern const uint8_t partition_flags[30];
extern const int frame_rate_tab[][2];
extern const uint8_t mv_scan[4];
static inline int xavs_clip(int a, int amin, int amax)
{
     if (a < amin)      return amin;
     else if (a > amax) return amax;
     else               return a;
}
static inline uint8_t xavs_clip_uint8(int a)
{
     if (a&(~255)) return (-a)>>31;
     else          return a;
}



static inline int clip3_int( int v, int i_min, int i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

static inline float clip3_float( float v, float f_min, float f_max )
{
    return ( (v < f_min) ? f_min : (v > f_max) ? f_max : v );
}
static inline int median( int a, int b, int c )
{
    int min = a, max =a;
    if( b < min )
        min = b;
    else
        max = b;    

    if( c < min )
        min = c;
    else if( c > max )
        max = c;

    return a + b + c - min - max;
}

static inline	uint32_t xavs_next_startcode(uint8_t *p_start,int i_len,int *p_ret)
{
	uint32_t i_startcode;
	uint8_t *p_buf=p_start;
	uint8_t *p_stop;
	p_stop=p_buf+i_len;
	if(i_len<4)
	{
		*p_ret=0;
		return 0;
	}
    i_startcode = p_buf[0];
	i_startcode = (i_startcode << 8) + p_buf[1];
	i_startcode = (i_startcode << 8) + p_buf[2];
	p_buf+=3;
	while(p_buf<p_stop)
	{
		i_startcode = (i_startcode << 8) + *p_buf;
		if((i_startcode&0xffffff00)==0x00000100)
		{
			*p_ret=(int)(p_buf-p_start)-3;
			return i_startcode;
		}
		p_buf++;
	}
	*p_ret=i_len-3;
	return 0;
}

static inline void copy_mvref_dir_type(xavs_mvref *mvref, xavs_block_size bsize)
{

	switch(bsize)
	{
    case BLK_16X16:
		mvref[MV_STRIDE  ].dir = mvref[0].dir;
		mvref[MV_STRIDE  ].type = mvref[0].type;
		mvref[MV_STRIDE + 1].dir = mvref[0].dir;
		mvref[MV_STRIDE + 1].type = mvref[0].type;
    case BLK_16X8:
		mvref[1].dir = mvref[0].dir;
		mvref[1].type = mvref[0].type;
		
        break;
    case BLK_8X16:
		mvref[MV_STRIDE].dir = mvref[0].dir;
		mvref[MV_STRIDE].type = mvref[0].type;
        break;
    }
}

static inline void copy_mvref(xavs_mvref *mvref, xavs_block_size bsize)
{

	switch(bsize)
	{
    case BLK_16X16:
		mvref[MV_STRIDE  ] = mvref[0];
		mvref[MV_STRIDE + 1] = mvref[0];
    case BLK_16X8:
		mvref[1] = mvref[0];
        break;
    case BLK_8X16:
		mvref[MV_STRIDE] = mvref[0];
        break;
    }
}
static inline void copy_mvs(xavs_vector *mv, enum xavs_block_size size) 
{

	//当size为BLK_16X16的时候相当于把X0的值赋给X1,X2,X3
	//当BLK_16X8时把X0的值赋给X1,X2的值赋给X3
	//当BLK_8X16时把X0的值赋给X2,X1的值赋给X3
    switch(size)
	{
    case BLK_16X16:
        mv[MV_STRIDE  ] = mv[0];
        mv[MV_STRIDE+1] = mv[0];
    case BLK_16X8:
        mv[1] = mv[0];
        break;
    case BLK_8X16:
        mv[MV_STRIDE] = mv[0];
        break;
    }
}


typedef struct tagxavs_aec_ctx
{
	unsigned char	mps;   
	unsigned char	cycno;
	unsigned int	lgPmps; 	
}xavs_aec_ctx;


typedef struct tagxavs_aec_decoder
{
	xavs_bitstream * stream;
	xavs_aec_ctx    *ctxes;
	int              ctx_count;

	uint32_t valueS,valueT;
	uint32_t rS1,rT1;


}xavs_aec_decoder;




typedef struct tagxavs_decoder
{
	xavs_bitstream                       s;//bit流读取结构
	xavs_video_sequence_header           vsh;//视频序列头
	xavs_picture_header                  ph;//图像头
	xavs_slice_header                    sh;//片头
	uint8_t                             *p_user_data;//用户数据
	xavs_sequence_display_extension     *p_sde;//序列显示扩展头
	xavs_copyright_extension            *p_ce;//版权扩展
	xavs_camera_parameters_extension    *p_cpe;//摄像机参数扩展
	xavs_picture_display_extension      *p_pde;//图像显示扩展
	uint8_t                              b_extention_flag;
	/*0 表示扩展数据之前是序列头，表示扩展之前是图象头*/
	uint32_t                              i_video_edit_code_flag;//说品编辑标志


	

	uint8_t                              b_get_video_sequence_header;//如果出现视频序列头则设置该标志
	uint8_t                              b_get_i_picture_header;//如果出现图像头则设置该标志
	uint8_t                              b_have_pred;//当前解码是否会采用帧间或场间预测
	uint32_t                             i_mb_width,i_mb_height,i_mb_num,i_mb_num_half;//宏块尺寸
	uint32_t                             i_mb_x,i_mb_y,i_mb_index,i_mb_offset;  //宏块的索引偏移等
	uint32_t                             b_complete; // 解码完一帧了，可以正常输出 
	uint32_t                             i_mb_flags; // 宏块标志表示该宏块周边块是否可以得到 
	uint32_t                             b_first_line; //片的第一行标志


	xavs_image image[3];//预先分配的3个帧图像，对应最多两个参考帧和一个当前解码帧，也可以分解出4个参考场，当前帧的2个场
	xavs_image ref[4];//四个参考场或者两个参考帧
	//
	xavs_image *p_save[3];//用于场编码的时候保存帧的使用
	xavs_image cur;//待解码的场
	uint8_t *p_y,*p_cb,*p_cr,*p_edge;//当前宏块YUV地址，p_edge为帧间预测时临时存放边缘像素的1/2或1/4像素的像素值
//
	//这些都是计算运动矢量的系数，用于加快速度
    int i_sym_factor;    ///<用于B块的对称模式
    int i_direct_den[4]; ///< 用于B块的直接模式
    int i_scale_den[4];  ///< 用于临近块的预测运动矢量的计算
	int i_ref_distance[4];

    int i_left_qp;
    uint8_t *p_top_qp;//用于宏块的环路滤波
	int i_qp;//当前使用的qp，
    int b_fixed_qp;//固定qp标志，可以在slice中改变
    int i_cbp;//当前宏块的cbp
	int i_luma_offset[4];//四个块相对于X0的地址偏移
    /** 
       0:    D3  B2  B3  C2
       4:    A1  X0  X1   -
       8:    A3  X2  X3   - */
	//24个保存前向后向各块运动矢量
    xavs_vector mv[24];
	xavs_vector *p_top_mv[2];//上面一行块的运动矢量
    xavs_vector *p_col_mv;//参考位置的对应运动矢量

	
	xavs_mvref mvref[24];//保存24个保存前向后向各块运动矢量 作为aec解码上下文参考
	xavs_mvref *p_top_mvref[2];//上面一行块的运动矢量 作为aec解码上下文参考

	xavs_mvd    mvd_a[4]; //针对aec使用,/标准中a块保存运动矢量差，作为aec解码上下文参考
	
    /** luma pred mode cache
       0:    D3  B2  B3
       3:    A1  X0  X1
       6:    A3  X2  X3   */
    int i_intra_pred_mode_y[9];//用于临时存贮上面九个块的亮度预测模式
    int *p_top_intra_pred_mode_y;//保存上面一行的预测模式

	int i_intra_pred_mode_chroma_a;//标准中a块位置的色度帧内预测模式
	int *p_top_intra_pred_mode_chroma;//保存上面一行的预测模式


	uint8_t   i_cbp_a;//标准中a块位置的cbp, 作为aec解码上下文参考
	uint8_t * p_top_cbp;//保存上面一行的预测模式

	uint8_t   i_mb_type_a;//标准中a块的宏块类型，作为aec解码上下文参考
	uint8_t  *p_top_mb_type;//上一行块宏块类型 


	int       i_last_qp;//上一个qp 作为aec解码上下文参考
	int       i_last_qp_delta;//上一个qp delta，作为aec解码上下文参考


    
	//这里宏块内不同的块对应的边缘不一样甚至用到刚解码的数据，或者由于有些右边块并未解码所以限制了一些预测模式的是使用
	//所有的这些都笔削在deblock前保存下来
	//用于保存帧内预测的当前宏块临近的上面行的y样本标准中c[0~16]
    uint8_t *p_top_border_y,*p_top_border_cb,*p_top_border_cr; 
	;//用于保存帧内预测的当前宏块临近的左边行的y样本
	uint8_t i_left_border_y[26],i_left_border_cb[10],i_left_border_cr[10];
	//用于中间边界样本的保存，用于X1,X3的预测
    uint8_t i_internal_border_y[26];
    uint8_t i_topleft_border_y,i_topleft_border_cb,i_topleft_border_cr;//保存标准中的c[0]或r[0]
    uint8_t *p_col_type_base;
	//用于保存宏块的类型，事实只有B_SKIP和B_Direct会用到，所以只需要保存P帧的就可以了
    uint8_t *p_col_type;//当前宏块类型的偏移地址	
//保存残差数据
    DCTELEM *p_block;


	uint8_t *p_wqM88;//加权量化矩阵
	//
	//
	unsigned char *data_buf;//输入缓冲区拷贝
	int  data_buf_len;//缓冲区实际有效字节
	int  data_buf_size;//缓冲区分配的大小
	int  data_buf_pos;//缓冲区现在有效数据起始位置

	xavs_aec_decoder *aec_decoder;//aec解码器

	
	int i_frame_skip_mode;//丢帧模式
}xavs_decoder;
int xavs_init_slice(xavs_decoder *p);
//int xavs_check_stream_end(xavs_decoder *p);


//vlc
int xavs_get_all_mbs(xavs_decoder *p);

///aec decoder
int xavs_aec_decoder_init(xavs_decoder *p);
int xavs_aec_decoder_get_all_mbs(xavs_decoder *p);
void xavs_aec_decoder_destroy(xavs_decoder *p);

void xavs_mb_init(xavs_decoder *p);
void xavs_mb_filter(xavs_decoder *p, int i_mb_type);
int  xavs_mb_next(xavs_decoder *p);



static int inline xavs_check_index(xavs_decoder *p)
{
	if(p->i_mb_index >= p->i_mb_num_half && p->i_mb_offset == 0 &&!p->ph.b_picture_structure)
	{
		return 1;
	}
	if(p->i_mb_index>=p->i_mb_num)
	{
		p->b_complete=1;
		return 1;
	}
	return 0;

}

static int inline xavs_check_stream_end(xavs_decoder *p)
{

	if(xavs_check_index(p))
	{
		return 1;
	}
	

	//if((p->i_mb_index%p->i_mb_width)==0)
	{
		if(xavs_bitstream_eof(&p->s))
		{
			return 1;
		}
	}
	return 0;
}


#endif 
